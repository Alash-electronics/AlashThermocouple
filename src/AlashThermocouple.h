// AlashThermocouple: термопары типа K через модули MAX6675 и MAX31855 (SPI): температура до 1024 / 1350 °C, температура холодного спая,
// определение обрыва и замыкания, проверка кадра. Программный или аппаратный SPI. Без зависимостей, только встроенная SPI.
// MIT License. Alash Electronics, https://github.com/Alash-electronics/AlashThermocouple
#ifndef ALASH_THERMOCOUPLE_H
#define ALASH_THERMOCOUPLE_H

#include <Arduino.h>
#include <SPI.h>

class AlashThermocouple {
 public:
  enum Chip : uint8_t { MAX6675 = 0, MAX31855 = 1 };
  enum Error : uint8_t {
    ERR_OK = 0,
    ERR_NO_DEVICE,       // модуль не отвечает: на MISO только единицы или нули
    ERR_OPEN,            // термопара не подключена или провод оборван
    ERR_SHORT_GND,       // выводы термопары замкнуты на GND (только MAX31855)
    ERR_SHORT_VCC,       // выводы термопары замкнуты на питание (только MAX31855)
    ERR_FAULT            // модуль сообщает о неисправности без подробностей
  };
  enum Language : uint8_t { LANG_RU = 0, LANG_KK = 1, LANG_EN = 2 };

  // Программный SPI на любых трёх пинах (удобно и для Uno, и для ESP)
  AlashThermocouple(Chip chip, uint8_t csPin, uint8_t clkPin, uint8_t misoPin) : _chip(chip), _cs(csPin), _clk(clkPin), _miso(misoPin), _soft(true) {}
  // Аппаратный SPI (пины SCK и MISO платы), нужен только CS
  AlashThermocouple(Chip chip, uint8_t csPin, SPIClass &spi) : _chip(chip), _cs(csPin), _clk(0), _miso(0), _spi(&spi), _soft(false) {}

  bool begin();                               // настроить пины; true, если модуль что-то ответил
  float temperature();                        // температура горячего спая (термопары), °C; NAN при ошибке
  float temperatureF() { float t = temperature(); return t * 1.8f + 32.0f; }
  float internalTemperature();                // температура модуля (холодный спай), °C: только MAX31855; у MAX6675 всегда NAN
  bool update();                              // принудительно прочитать модуль
  void setCacheMs(uint16_t ms) { _cacheMs = ms; }   // как долго читать не нужно: по умолчанию 250 мс для MAX6675 (измерение 0.22 с), 100 мс для MAX31855
  void setOffset(float degrees) { _offset = degrees; }   // поправка по эталону
  float offset() const { return _offset; }
  uint32_t raw() const { return _raw; }       // последний сырой кадр (16 или 32 бита)

  Error error() const { return _err; }
  void clearError() { _err = ERR_OK; }
  void setLanguage(Language lang) { _lang = lang; }
  const __FlashStringHelper *errorText();
  void diagnose(Print &out);

  // Разбор кадра: вынесен для тестов. true, если кадр правдоподобен; err сообщает о неисправности (при true температура тоже NAN)
  static bool decode6675(uint16_t frame, float &tc, Error &err);
  static bool decode31855(uint32_t frame, float &tc, float &internal, Error &err);

 private:
  uint32_t readFrame();
  uint8_t shiftByte();
  Chip _chip; uint8_t _cs, _clk, _miso; SPIClass *_spi = nullptr; bool _soft;
  uint16_t _cacheMs = 0; bool _cacheSet = false, _has = false;
  uint32_t _readAt = 0, _raw = 0;
  float _tc = NAN, _int = NAN, _offset = 0;
  Error _err = ERR_OK; Language _lang = LANG_RU;
};

#endif
