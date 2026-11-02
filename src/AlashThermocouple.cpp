// MIT License. Alash Electronics.
#include "AlashThermocouple.h"

#define T(ru, kk, en) (_lang == LANG_KK ? F(kk) : _lang == LANG_EN ? F(en) : F(ru))

bool AlashThermocouple::begin() {
  pinMode(_cs, OUTPUT);
  digitalWrite(_cs, HIGH);
  if (_soft) {
    pinMode(_clk, OUTPUT);
    digitalWrite(_clk, LOW);
    pinMode(_miso, INPUT);
  } else {
    _spi->begin();
  }
  if (!_cacheSet) _cacheMs = _chip == MAX6675 ? 250 : 100;
  _has = false;
  return update() || _err != ERR_NO_DEVICE;
}

uint8_t AlashThermocouple::shiftByte() {
  uint8_t d = 0;
  for (int8_t i = 7; i >= 0; i--) {
    digitalWrite(_clk, HIGH);
    delayMicroseconds(2);
    if (digitalRead(_miso)) d |= (1 << i);     // данные меняются по спаду, читаем по фронту после паузы
    digitalWrite(_clk, LOW);
    delayMicroseconds(2);
  }
  return d;
}

// Один кадр: 16 бит у MAX6675 и 32 бита у MAX31855, старший бит первым, SPI режим 0.
uint32_t AlashThermocouple::readFrame() {
  uint8_t n = _chip == MAX6675 ? 2 : 4;
  uint32_t v = 0;
  digitalWrite(_cs, LOW);
  delayMicroseconds(5);
  if (_soft) {
    for (uint8_t i = 0; i < n; i++) v = (v << 8) | shiftByte();
  } else {
    _spi->beginTransaction(SPISettings(1000000, MSBFIRST, SPI_MODE0));
    for (uint8_t i = 0; i < n; i++) v = (v << 8) | _spi->transfer(0);
    _spi->endTransaction();
  }
  digitalWrite(_cs, HIGH);
  return v;
}

bool AlashThermocouple::decode6675(uint16_t f, float &tc, Error &err) {
  tc = NAN; err = ERR_OK;
  if (f == 0xFFFF || (f & 0x8000) || (f & 0x0002)) { err = ERR_NO_DEVICE; return false; }    // бит 15 всегда 0, бит 1 (ID устройства) всегда 0
  if (f & 0x0004) { err = ERR_OPEN; return true; }                                           // бит 2: термопара не подключена
  tc = (f >> 3) * 0.25f;                                                                     // 12 бит, 0.25 °C
  return true;
}

bool AlashThermocouple::decode31855(uint32_t f, float &tc, float &in, Error &err) {
  tc = NAN; in = NAN; err = ERR_OK;
  if (f == 0xFFFFFFFFUL || f == 0) { err = ERR_NO_DEVICE; return false; }
  // температура модуля: биты 15..4, 12 бит со знаком, 0.0625 °C (доступна и при неисправности термопары)
  int16_t it = (int16_t)((f >> 4) & 0xFFF);
  if (it & 0x800) it |= (int16_t)0xF000;
  in = it * 0.0625f;
  if (f & 0x00010000UL) {                                                                    // бит 16: неисправность; подробности в битах 2..0
    if (f & 0x1) err = ERR_OPEN;
    else if (f & 0x2) err = ERR_SHORT_GND;
    else if (f & 0x4) err = ERR_SHORT_VCC;
    else err = ERR_FAULT;
    return true;
  }
  // термопара: биты 31..18, 14 бит со знаком, 0.25 °C
  int16_t t14 = (int16_t)((f >> 18) & 0x3FFF);
  if (t14 & 0x2000) t14 |= (int16_t)0xC000;
  tc = t14 * 0.25f;
  return true;
}

bool AlashThermocouple::update() {
  _raw = readFrame();
  bool plausible;
  if (_chip == MAX6675) { plausible = decode6675((uint16_t)_raw, _tc, _err); _int = NAN; }
  else plausible = decode31855(_raw, _tc, _int, _err);
  _readAt = millis(); _has = true;
  return plausible && _err == ERR_OK;
}

float AlashThermocouple::temperature() {
  if (!_has || (uint32_t)(millis() - _readAt) >= _cacheMs) update();
  return _tc == _tc ? _tc + _offset : NAN;
}

float AlashThermocouple::internalTemperature() {
  if (!_has || (uint32_t)(millis() - _readAt) >= _cacheMs) update();
  return _int;
}

const __FlashStringHelper *AlashThermocouple::errorText() {
  switch (_err) {
    case ERR_OK: return F("OK");
    case ERR_NO_DEVICE:
      return T("Модуль не отвечает (на линии MISO одни единицы или нули). Проверьте питание модуля (3.3-5 В), провода CS, SCK (CLK), SO (MISO) и GND, и что CS, CLK и MISO указаны в программе верно.",
               "Модуль жауап бермейді (MISO желісінде тек бірліктер немесе нөлдер). Модуль қуатын (3.3-5 В), CS, SCK (CLK), SO (MISO) және GND сымдарын, сондай-ақ CS, CLK және MISO бағдарламада дұрыс көрсетілгенін тексеріңіз.",
               "The module does not answer (the MISO line is all ones or all zeros). Check the module power (3.3-5 V), the CS, SCK (CLK), SO (MISO) and GND wires, and that CS, CLK and MISO are given correctly in the program.");
    case ERR_OPEN:
      return T("Термопара не подключена или провод оборван. Проверьте клеммы T+ и T- на модуле и целостность провода у спая.",
               "Термопара қосылмаған немесе сым үзілген. Модульдегі T+ және T- қыстырғыштарын және түйін жанындағы сым бүтіндігін тексеріңіз.",
               "The thermocouple is not connected or the wire is broken. Check the T+ and T- terminals on the module and the wire near the junction.");
    case ERR_SHORT_GND:
      return T("Выводы термопары замкнуты на GND. Проверьте изоляцию проводов и клеммы.",
               "Термопара шықпалары GND-ға тұйықталған. Сымдардың оқшаулауын және қыстырғыштарды тексеріңіз.",
               "The thermocouple leads are shorted to GND. Check the wire insulation and the terminals.");
    case ERR_SHORT_VCC:
      return T("Выводы термопары замкнуты на питание модуля. Проверьте изоляцию проводов и клеммы.",
               "Термопара шықпалары модуль қуатына тұйықталған. Сымдардың оқшаулауын және қыстырғыштарды тексеріңіз.",
               "The thermocouple leads are shorted to the module supply. Check the wire insulation and the terminals.");
    case ERR_FAULT:
      return T("Модуль сообщает о неисправности термопары. Проверьте провода и клеммы.",
               "Модуль термопара ақауы туралы хабарлайды. Сымдар мен қыстырғыштарды тексеріңіз.",
               "The module reports a thermocouple fault. Check the wires and terminals.");
  }
  return F("");
}

void AlashThermocouple::diagnose(Print &out) {
  out.println(T("AlashThermocouple: диагностика", "AlashThermocouple: диагностика", "AlashThermocouple: diagnostics"));
  out.println(_chip == MAX6675 ? F("Chip: MAX6675 (0..1023.75 C, 0.25 C)") : F("Chip: MAX31855 (-200..1350 C, 0.25 C)"));
  if (_soft) { out.print(F("CS=")); out.print((long)_cs); out.print(F(" CLK=")); out.print((long)_clk); out.print(F(" MISO=")); out.println((long)_miso); }
  else { out.print(F("CS=")); out.print((long)_cs); out.println(T(" (аппаратный SPI)", " (аппараттық SPI)", " (hardware SPI)")); }
  update();
  out.print(T("Сырой кадр: 0x", "Шикі кадр: 0x", "Raw frame: 0x")); out.println((unsigned long)_raw, HEX);
  if (_err != ERR_OK) out.println(errorText());
  if (_tc == _tc) { out.print(T("Температура термопары: ", "Термопара температурасы: ", "Thermocouple: ")); out.print(_tc, 2); out.println(F(" C")); }
  if (_int == _int) { out.print(T("Температура модуля (холодный спай): ", "Модуль температурасы (суық түйін): ", "Module (cold junction): ")); out.print(_int, 2); out.println(F(" C")); }
  out.println(T("Подсказки: термопара типа K полярна: на клемму T+ идёт плюсовой провод (по стандарту ANSI жёлтый плюс и красный минус, по IEC зелёный плюс и белый минус). Если температура падает при нагреве, поменяйте провода местами. Не прокладывайте провода рядом с силовыми цепями нагревателя. Читать модуль чаще, чем раз в 0.25 с (MAX6675) или 0.1 с (MAX31855) бессмысленно: измерение занимает это время. MAX6675 не измеряет ниже 0 градусов.",
                "Кеңестер: K типті термопара полярлы: T+ қыстырғышына плюс сым қосылады (ANSI стандарты бойынша сары плюс және қызыл минус, IEC бойынша жасыл плюс және ақ минус). Қыздырғанда температура төмендесе, сымдарды ауыстырыңыз. Сымдарды қыздырғыштың күштік тізбегінің жанына жүргізбеңіз. Модульді 0.25 с (MAX6675) немесе 0.1 с (MAX31855) сайын жиі оқудың мәні жоқ. MAX6675 0 градустан төмен өлшемейді.",
                "Tips: a type K thermocouple is polarized: the plus lead goes to the T+ terminal (ANSI: yellow plus and red minus, IEC: green plus and white minus). If the temperature falls while heating, swap the wires. Do not run the wires beside the heater power circuit. Reading the module more often than every 0.25 s (MAX6675) or 0.1 s (MAX31855) is pointless: a measurement takes that long. The MAX6675 does not measure below 0 degrees."));
}
