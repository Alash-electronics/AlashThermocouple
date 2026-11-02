// [ru] AlashThermocouple: модуль не работает или температура неверная? Скетч покажет сырой кадр, температуры термопары и модуля и подскажет, что проверить.
//      Язык подсказок: LANG_RU, LANG_KK или LANG_EN.
// [kk] AlashThermocouple: модуль жұмыс істемей ме немесе температура қате ме? Скетч шикі кадрды, термопара мен модуль температураларын көрсетеді және нені тексеру керегін айтады.
//      Кеңес тілі: LANG_RU, LANG_KK немесе LANG_EN.
// [en] AlashThermocouple: the module does not work or the temperature is wrong? The sketch shows the raw frame, the thermocouple and module temperatures and tells what to check.
//      Tips language: LANG_RU, LANG_KK or LANG_EN.
#include <AlashThermocouple.h>

AlashThermocouple tc(AlashThermocouple::MAX6675, 5, 6, 4);

void setup() {
  Serial.begin(115200);
  tc.setLanguage(AlashThermocouple::LANG_RU);
  tc.begin();
  tc.diagnose(Serial);
}

void loop() {}
