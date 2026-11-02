// [ru] AlashThermocouple: самый простой пример. Температура термопары типа K через модуль MAX6675 (или MAX31855) раз в секунду.
//
//      Подключение модуля MAX6675 (программный SPI, пины любые): VCC к 3.3 В или 5 В, GND к GND, SCK к пину 6, CS к пину 5, SO к пину 4.
//      Термопара к клеммам модуля: плюсовой провод к T+ (по ANSI жёлтый), минус к T-. Для MAX31855 замените MAX6675 на MAX31855.
// [kk] AlashThermocouple: ең қарапайым мысал. MAX6675 (немесе MAX31855) модулі арқылы K типті термопара температурасы секунд сайын.
//
//      MAX6675 модулін қосу (бағдарламалық SPI, пиндер кез келген): VCC 3.3 В немесе 5 В-қа, GND GND-ға, SCK 6 пинге, CS 5 пинге, SO 4 пинге.
//      Термопара модуль қыстырғыштарына: плюс сым T+-қа (ANSI бойынша сары), минус T--қа. MAX31855 үшін MAX6675 орнына MAX31855 жазыңыз.
// [en] AlashThermocouple: the simplest example. The temperature of a type K thermocouple through a MAX6675 (or MAX31855) module once a second.
//
//      MAX6675 module wiring (software SPI, any pins): VCC to 3.3 V or 5 V, GND to GND, SCK to pin 6, CS to pin 5, SO to pin 4.
//      The thermocouple to the module terminals: the plus lead to T+ (yellow in ANSI), the minus to T-. For a MAX31855 replace MAX6675 with MAX31855.
#include <AlashThermocouple.h>

AlashThermocouple tc(AlashThermocouple::MAX6675, 5, 6, 4);   // чип, CS, SCK, SO(MISO)

void setup() {
  Serial.begin(115200);
  if (!tc.begin()) Serial.println(tc.errorText());
}

void loop() {
  float t = tc.temperature();            // °C, NAN при ошибке
  if (t == t) { Serial.print(t, 2); Serial.println(F(" C")); }
  else Serial.println(tc.errorText());   // обрыв, замыкание, нет модуля
  delay(1000);
}
