// Тесты AlashThermocouple без железа: имитация шины и микросхем MAX6675 / MAX31855 (биты сдвигаются по спаду SCK, как на настоящих).
//   g++ -std=c++17 -Wall -fsanitize=address,undefined -I mock -I ../../src test_thermocouple.cpp ../../src/AlashThermocouple.cpp && ./a.out
#include <stdio.h>
#include "Arduino.h"
#include "SPI.h"
#include "AlashThermocouple.h"

static int fails = 0;
#define CHECK(cond) do { if (!(cond)) { printf("FAIL line %d: %s\n", __LINE__, #cond); fails++; } } while (0)
static bool near(float a, float b, float e) { return fabsf(a - b) <= e; }
typedef AlashThermocouple A;

static uint16_t f6675(float c, bool open = false) { return (uint16_t)(((int)(c / 0.25f) & 0xFFF) << 3) | (open ? 4 : 0); }
static uint32_t f31855(float tc, float in, uint32_t faultBits = 0) {
  uint32_t t = (uint32_t)((int32_t)(tc / 0.25f) & 0x3FFF), i = (uint32_t)((int32_t)(in / 0.0625f) & 0xFFF);
  return (t << 18) | (faultBits ? 0x10000u : 0) | (i << 4) | faultBits;
}
static void bus(uint32_t frame, int nbits) { g_bus.reset(); g_bus.cs = 10; g_bus.clk = 11; g_bus.miso = 12; g_bus.frame = frame; g_bus.nbits = nbits; g_ms += 5000; }

int main() {
  // 1) разбор кадров по таблицам даташитов (MAX31855: +1600, +0.25, 0, -0.25, -250; внутренняя 127, 25, 0.0625, -0.0625, -55)
  { float tc, in; A::Error e;
    struct { uint32_t t14; float v; } tcs[] = {{0x1900, 1600}, {0x0001, 0.25f}, {0, 0}, {0x3FFF, -0.25f}, {0x3C18, -250}};
    for (auto &c : tcs) { CHECK(A::decode31855(c.t14 << 18 | (0x190 << 4), tc, in, e) && e == A::ERR_OK && near(tc, c.v, 0.001f)); }
    struct { uint32_t t12; float v; } ins[] = {{0x7F0, 127}, {0x190, 25}, {0x001, 0.0625f}, {0xFFF, -0.0625f}, {0xC90, -55}};
    for (auto &c : ins) { CHECK(A::decode31855(((uint32_t)0x64 << 18) | (c.t12 << 4), tc, in, e) && near(in, c.v, 0.0001f)); }
    CHECK(A::decode31855(((uint32_t)0x64 << 18) | (0x190 << 4), tc, in, e) && near(tc, 25.0f, 0.001f) && near(in, 25.0f, 0.001f));
    // неисправности
    uint32_t base = ((uint32_t)0x64 << 18) | (0x190 << 4) | 0x10000;
    CHECK(A::decode31855(base | 1, tc, in, e) && e == A::ERR_OPEN && isnan(tc) && near(in, 25.0f, 0.001f));   // температура модуля доступна и при обрыве
    CHECK(A::decode31855(base | 2, tc, in, e) && e == A::ERR_SHORT_GND);
    CHECK(A::decode31855(base | 4, tc, in, e) && e == A::ERR_SHORT_VCC);
    CHECK(A::decode31855(base, tc, in, e) && e == A::ERR_FAULT);
    CHECK(!A::decode31855(0xFFFFFFFFu, tc, in, e) && e == A::ERR_NO_DEVICE && !A::decode31855(0, tc, in, e) && e == A::ERR_NO_DEVICE);
    // MAX6675: 25 °C = 100 единиц; 1023.75 = 4095; обрыв; нет модуля
    CHECK(A::decode6675(100 << 3, tc, e) && e == A::ERR_OK && near(tc, 25.0f, 0.001f));
    CHECK(A::decode6675(0x7FF8, tc, e) && near(tc, 1023.75f, 0.001f));
    CHECK(A::decode6675(0, tc, e) && near(tc, 0, 0.001f));
    CHECK(A::decode6675((100 << 3) | 4, tc, e) && e == A::ERR_OPEN && isnan(tc));
    CHECK(!A::decode6675(0xFFFF, tc, e) && e == A::ERR_NO_DEVICE && !A::decode6675(0x8000, tc, e) && !A::decode6675(0x0002, tc, e));
  }

  // 2) MAX6675 по программному SPI: значение, градусы Фаренгейта, поправка
  { bus(f6675(212.5f), 16);
    A t(A::MAX6675, 10, 11, 12);
    CHECK(t.begin() && near(t.temperature(), 212.5f, 0.001f) && t.error() == A::ERR_OK);
    CHECK(near(t.temperatureF(), 414.5f, 0.01f) && isnan(t.internalTemperature()));
    CHECK(g_bus.clockEdgesWhileCsHigh == 0 && g_bus.frames == 1);                  // читали один раз, такты только при CS = LOW
    t.setOffset(-1.5f); CHECK(near(t.temperature(), 211.0f, 0.001f) && t.offset() == -1.5f);
    CHECK((uint16_t)t.raw() == f6675(212.5f));
  }

  // 3) MAX31855: температура, внутренняя, отрицательные значения
  { bus(f31855(-50.25f, 23.5f), 32);
    A t(A::MAX31855, 10, 11, 12);
    CHECK(t.begin() && near(t.temperature(), -50.25f, 0.001f) && near(t.internalTemperature(), 23.5f, 0.001f));
    bus(f31855(1300.0f, -12.5f), 32); g_ms += 1000;
    CHECK(near(t.temperature(), 1300.0f, 0.001f) && near(t.internalTemperature(), -12.5f, 0.001f));
  }

  // 4) кэш: между чтениями меньше интервала модуль не опрашивается, потом опрашивается
  { bus(f31855(100, 20), 32); A t(A::MAX31855, 10, 11, 12); t.begin(); int n = g_bus.frames;
    t.temperature(); t.temperature(); CHECK(g_bus.frames == n);
    g_ms += 99; t.temperature(); CHECK(g_bus.frames == n);
    g_ms += 2; g_bus.frame = f31855(120, 20); t.temperature(); CHECK(g_bus.frames == n + 1 && near(t.temperature(), 120, 0.001f));   // по умолчанию 100 мс
    bus(f6675(50), 16); A s(A::MAX6675, 10, 11, 12); s.begin(); int m = g_bus.frames; g_ms += 200; s.temperature(); CHECK(g_bus.frames == m);
    g_ms += 60; s.temperature(); CHECK(g_bus.frames == m + 1);                                                     // 250 мс у MAX6675
    s.setCacheMs(1000); g_ms += 500; s.temperature(); CHECK(g_bus.frames == m + 1);
  }

  // 5) обрыв, замыкание: NAN и понятная причина, потом починили
  { bus(f6675(30, true), 16); A t(A::MAX6675, 10, 11, 12);
    CHECK(t.begin() && isnan(t.temperature()) && t.error() == A::ERR_OPEN);          // модуль есть, термопары нет
    g_bus.frame = f6675(30); g_ms += 500; CHECK(near(t.temperature(), 30, 0.001f) && t.error() == A::ERR_OK);
    bus(f31855(0, 21, 2), 32); A k(A::MAX31855, 10, 11, 12); k.begin();
    CHECK(isnan(k.temperature()) && k.error() == A::ERR_SHORT_GND && near(k.internalTemperature(), 21, 0.001f));
  }

  // 6) модуля нет: линия MISO в воздухе читается единицами, а притянутая к земле нулями
  { bus(0, 16); g_bus.floating = true; g_bus.floatHigh = true; A t(A::MAX6675, 10, 11, 12);
    CHECK(!t.begin() && t.error() == A::ERR_NO_DEVICE && isnan(t.temperature()));
    bus(0, 32); g_bus.floating = true; g_bus.floatHigh = false; A k(A::MAX31855, 10, 11, 12);
    CHECK(!k.begin() && k.error() == A::ERR_NO_DEVICE);
  }

  // 7) аппаратный SPI: режим 0, байты по порядку, значения те же
  { bus(f31855(88.75f, 30.0f), 32); g_spiBegins = 0; g_spiTransactions = 0;
    A t(A::MAX31855, 10, SPI);
    CHECK(t.begin() && g_spiBegins == 1 && near(t.temperature(), 88.75f, 0.001f) && near(t.internalTemperature(), 30.0f, 0.001f));
    CHECK(g_lastSettings.mode == SPI_MODE0 && g_lastSettings.order == MSBFIRST && g_lastSettings.clock <= 4000000 && g_spiTransactions == 1);
    bus(f6675(500), 16); A s(A::MAX6675, 10, SPI); CHECK(s.begin() && near(s.temperature(), 500, 0.001f));
  }

  // 8) тексты ошибок на трёх языках, диагностика
  { A::Error list[] = {A::ERR_NO_DEVICE, A::ERR_OPEN, A::ERR_SHORT_GND, A::ERR_SHORT_VCC, A::ERR_FAULT};
    uint32_t frames[] = {0, 0, f31855(0, 21, 2), f31855(0, 21, 4), f31855(0, 21, 8)};
    for (int i = 0; i < 5; i++) {
      if (i == 0) { bus(0, 32); g_bus.floating = true; g_bus.floatHigh = true; }
      else if (i == 1) bus(f6675(20, true), 16);
      else bus(frames[i], 32);
      A t(i == 1 ? A::MAX6675 : A::MAX31855, 10, 11, 12); t.begin();
      CHECK(t.error() == list[i]);
      t.setLanguage(A::LANG_RU); const char *ru = (const char *)t.errorText();
      t.setLanguage(A::LANG_KK); const char *kk = (const char *)t.errorText();
      t.setLanguage(A::LANG_EN); const char *en = (const char *)t.errorText();
      CHECK(strlen(ru) > 20 && strlen(kk) > 20 && strlen(en) > 20 && strcmp(ru, kk) && strcmp(kk, en) && strcmp(ru, en));
    }
    bus(f31855(25, 22), 32); A d(A::MAX31855, 10, 11, 12); d.begin(); d.setLanguage(A::LANG_EN); Print p; d.diagnose(p);
    CHECK(p.text.find("MAX31855") != std::string::npos && p.text.find("CS=10 CLK=11 MISO=12") != std::string::npos && p.text.find("Thermocouple: 25.00") != std::string::npos && p.text.find("Module (cold junction): 22.00") != std::string::npos && p.text.find("Raw frame: 0x") != std::string::npos);
    d.setLanguage(A::LANG_KK); Print pk; d.diagnose(pk); CHECK(pk.text.find("Термопара температурасы: 25.00") != std::string::npos);
    A ok(A::MAX6675, 10, SPI); CHECK(strcmp((const char *)ok.errorText(), "OK") == 0);
  }

  if (fails) { printf("FAILED: %d\n", fails); return 1; }
  printf("ALL TESTS PASSED\n");
  return 0;
}
