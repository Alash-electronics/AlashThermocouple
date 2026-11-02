// [ru] AlashThermocouple: термостат печи или паяльной станции. Реле (пин 7, активный LOW) греет, пока температура ниже 200 градусов,
//      и отключает на 205: гистерезис 5 градусов не даёт реле щёлкать. При обрыве термопары нагрев ВЫКЛЮЧАЕТСЯ: нельзя греть вслепую.
//      ВНИМАНИЕ: нагреватель от сети 220 В опасен. Для уроков используйте низкое напряжение. Подключение датчика как в примере Basic.
// [kk] AlashThermocouple: пеш немесе дәнекерлеу станциясының термостаты. Реле (7 пин, белсенді LOW) температура 200 градустан төмен болғанда қыздырады
//      және 205-те өшіреді: 5 градус гистерезис релені шиқылдатпайды. Термопара үзілсе қыздыру ӨШІРІЛЕДІ: соқыр қыздыруға болмайды.
//      НАЗАР АУДАРЫҢЫЗ: 220 В желіден қыздырғыш қауіпті. Сабақтарда төмен кернеуді қолданыңыз. Датчикті қосу Basic мысалындағыдай.
// [en] AlashThermocouple: a thermostat for a kiln or a soldering station. The relay (pin 7, active LOW) heats while the temperature is below 200 degrees
//      and cuts off at 205: a 5 degree hysteresis keeps the relay from clicking. If the thermocouple breaks, the heating switches OFF: you must not heat blind.
//      WARNING: a 220 V mains heater is dangerous. For lessons use low voltage. The sensor wiring is as in the Basic example.
#include <AlashThermocouple.h>

AlashThermocouple tc(AlashThermocouple::MAX31855, 5, 6, 4);
const uint8_t RELAY = 7;
const float LOW_T = 200.0f, HIGH_T = 205.0f;
bool heating = false;

void setup() {
  Serial.begin(115200);
  pinMode(RELAY, OUTPUT);
  digitalWrite(RELAY, HIGH);             // реле выключено (активный LOW)
  tc.begin();
}

void loop() {
  float t = tc.temperature();
  if (t != t) {                          // нет достоверной температуры: нагрев выключаем
    heating = false;
    Serial.println(tc.errorText());
  } else if (t < LOW_T) heating = true;
  else if (t > HIGH_T) heating = false;
  digitalWrite(RELAY, heating ? LOW : HIGH);
  if (t == t) { Serial.print(t, 1); Serial.println(heating ? F(" C  HEAT") : F(" C")); }
  delay(250);
}
