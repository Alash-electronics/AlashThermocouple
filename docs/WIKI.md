# AlashThermocouple: руководство / нұсқаулық / guide

[Русский](#русский) | [Қазақша](#қазақша) | [English](#english)

---

## Русский

Простая библиотека для **термопар типа K** через модули **MAX6675** (0..1024 °C) и **MAX31855** (-200..1350 °C). Температура печи, паяльника, свечи, выхлопа. Находит обрыв и замыкание, показывает температуру холодного спая, объясняет, что не так. Программный SPI на любых пинах или аппаратный. Только встроенная `SPI`. Сделана для уроков и наборов Alash Electronics.

### Установка
Arduino IDE: Скетч, Подключить библиотеку, Управлять библиотеками, найти **AlashThermocouple** и нажать Установить. Или скачайте ZIP с GitHub: Скетч, Подключить библиотеку, Добавить .ZIP библиотеку.

### Платы
Arduino Uno / Nano / Mega (AVR), ESP32, ESP8266 и другие, где работает Arduino-ядро.

### Подключение
| MAX6675 / MAX31855 | Uno (пример) | ESP32 |
|---|---|---|
| VCC | 3.3 В или 5 В | 3.3 В |
| GND | GND | GND |
| SCK | пин 6 | любой пин |
| CS | пин 5 | любой пин |
| SO | пин 4 | любой пин |
| T+ / T- | термопара (плюс на T+) | термопара |

Пины задаются в программе (программный SPI) или берутся аппаратные (`AlashThermocouple tc(MAX31855, CS, SPI)`). Термопара полярна: по ANSI жёлтый плюс и красный минус, по IEC зелёный плюс и белый минус. Провода термопары не прокладывайте рядом с силовыми цепями нагревателя.

### Быстрый старт
```cpp
#include <AlashThermocouple.h>
AlashThermocouple tc(AlashThermocouple::MAX6675, 5, 6, 4);   // чип, CS, SCK, SO

void setup() { Serial.begin(115200); tc.begin(); }
void loop() {
  Serial.println(tc.temperature());   // °C, NAN при обрыве
  delay(1000);
}
```

### Что умеет
- **`temperature()` в одну строку:** °C (и °F), `NAN` при неисправности.
- **MAX6675 и MAX31855:** один класс, чип выбирается параметром.
- **Обрыв и замыкание:** `ERR_OPEN`, `ERR_SHORT_GND`, `ERR_SHORT_VCC` (MAX31855), `ERR_NO_DEVICE`, если модуль не отвечает.
- **Холодный спай:** `internalTemperature()` у MAX31855, доступна даже при обрыве.
- **Программный или аппаратный SPI:** любые три пина или SPI платы.
- **Кэш:** модуль не опрашивается чаще, чем он измеряет (250 мс и 100 мс), `setCacheMs()`.
- **Поправка:** `setOffset()` по эталонному термометру.
- **Видно, что происходит:** `diagnose(Serial)` показывает сырой кадр и подсказки (полярность, помехи, питание) на трёх языках.
- Работает на **Arduino AVR, ESP32, ESP8266**.

### Справочник по методам
| Метод | Что делает |
|---|---|
| `AlashThermocouple(chip, cs, clk, miso)` | Программный SPI: чип MAX6675 или MAX31855 и три пина |
| `AlashThermocouple(chip, cs, SPI)` | Аппаратный SPI: только CS |
| `begin()` | Настроить пины; false, если модуль не отвечает |
| `temperature() / temperatureF()` | Температура термопары, NAN при ошибке |
| `internalTemperature()` | Холодный спай (MAX31855) |
| `setOffset(deg) / setCacheMs(ms)` | Поправка / интервал опроса |
| `update() / raw()` | Прочитать сейчас / последний сырой кадр |
| `error() / errorText() / setLanguage()` | Ошибка и сообщение на ru/kk/en |
| `diagnose(Serial)` | Самопроверка с подсказками |

### Если что-то не работает
**Ошибка «термопара не подключена» (ERR_OPEN)**

Проверьте клеммы T+ и T- на модуле и целостность провода у самого спая: термопары часто ломаются у места изгиба. Модуль без термопары всегда даёт этот код.

**Температура падает, когда должна расти**

Перепутана полярность термопары: поменяйте провода на клеммах T+ и T- (по ANSI жёлтый плюс, красный минус; по IEC зелёный плюс, белый минус).

**Температура скачет или показывает ерунду**

Помехи от силовых цепей нагревателя или реле: уведите провода термопары от силовых, добавьте конденсатор 0.1 мкФ между T+ и T-, питайте модуль отдельно. Модуль MAX6675 нужно опрашивать не чаще раза в 0.25 с.

**Ошибка «модуль не отвечает» (ERR_NO_DEVICE)**

На линии SO только единицы или нули: проверьте питание модуля, провода CS, SCK, SO и GND, и что номера пинов в программе совпадают с подключением. У ESP32 и ESP8266 лучше использовать пины общего назначения (не 34-39 для выходов CS и SCK).

**MAX6675 не показывает температуру ниже нуля**

Это ограничение микросхемы: она измеряет 0..1023.75 °C. Для отрицательных температур возьмите MAX31855 (-200..1350 °C).

### Самопроверка
В библиотеке есть `diagnose(Serial)`: она печатает, что видит плата, и подсказывает, что проверить. Язык подсказок: `setLanguage(LANG_RU / LANG_KK / LANG_EN)`.

### Примеры
`Basic`, `Thermostat`, `Diagnose`

---

## Қазақша

**MAX6675** (0..1024 °C) және **MAX31855** (-200..1350 °C) модульдері арқылы **K типті термопараларға** арналған қарапайым кітапхана. Пеш, дәнекерлегіш, шам, түтін температурасы. Үзілу мен тұйықталуды табады, суық түйін температурасын көрсетеді, не дұрыс емесін түсіндіреді. Кез келген пиндегі бағдарламалық SPI немесе аппараттық. Тек кірістірілген `SPI`.

### Орнату
Arduino IDE: Sketch, Include Library, Manage Libraries, **AlashThermocouple** табып, Install басыңыз. Немесе GitHub-тан ZIP жүктеп, Sketch, Include Library, Add .ZIP Library.

### Тақталар
Arduino Uno / Nano / Mega (AVR), ESP32, ESP8266 және Arduino ядросы бар басқалар.

### Қосу
| MAX6675 / MAX31855 | Uno (пример) | ESP32 |
|---|---|---|
| VCC | 3.3 В или 5 В | 3.3 В |
| GND | GND | GND |
| SCK | пин 6 | любой пин |
| CS | пин 5 | любой пин |
| SO | пин 4 | любой пин |
| T+ / T- | термопара (плюс на T+) | термопара |

Пиндер бағдарламада беріледі (бағдарламалық SPI) немесе аппараттық алынады (`AlashThermocouple tc(MAX31855, CS, SPI)`). Термопара полярлы: ANSI бойынша сары плюс және қызыл минус, IEC бойынша жасыл плюс және ақ минус. Термопара сымдарын қыздырғыштың күштік тізбегінің жанына жүргізбеңіз.

### Жылдам бастау
```cpp
#include <AlashThermocouple.h>
AlashThermocouple tc(AlashThermocouple::MAX6675, 5, 6, 4);   // чип, CS, SCK, SO

void setup() { Serial.begin(115200); tc.begin(); }
void loop() {
  Serial.println(tc.temperature());   // °C, NAN при обрыве
  delay(1000);
}
```

### Мүмкіндіктері
- **`temperature()` бір жолда:** °C (және °F), ақау болса `NAN`.
- **MAX6675 және MAX31855:** бір класс, чип параметрмен таңдалады.
- **Үзілу және тұйықталу:** `ERR_OPEN`, `ERR_SHORT_GND`, `ERR_SHORT_VCC` (MAX31855), модуль жауап бермесе `ERR_NO_DEVICE`.
- **Суық түйін:** MAX31855-те `internalTemperature()`, үзілгенде де қолжетімді.
- **Бағдарламалық немесе аппараттық SPI:** кез келген үш пин немесе тақта SPI.
- **Кэш:** модуль өлшегеннен жиі сұралмайды (250 мс және 100 мс), `setCacheMs()`.
- **Түзету:** `setOffset()` эталон термометр бойынша.
- **Не болып жатқаны көрінеді:** `diagnose(Serial)` шикі кадрды және үш тілде кеңестерді көрсетеді (полярлық, кедергі, қуат).
- **Arduino AVR, ESP32, ESP8266** тақталарында жұмыс істейді.

### Әдістер анықтамасы
| Әдіс | Не істейді |
|---|---|
| `AlashThermocouple(chip, cs, clk, miso)` | Бағдарламалық SPI |
| `AlashThermocouple(chip, cs, SPI)` | Аппараттық SPI |
| `begin()` | Пиндерді баптау |
| `temperature() / temperatureF()` | Термопара температурасы |
| `internalTemperature()` | Суық түйін |
| `setOffset(deg) / setCacheMs(ms)` | Түзету / сұрау аралығы |
| `update() / raw()` | Қазір оқу / шикі кадр |
| `error() / errorText() / setLanguage()` | Қате және хабарлама |
| `diagnose(Serial)` | Кеңестермен өзін-өзі тексеру |

### Бірдеңе жұмыс істемесе
**«Термопара қосылмаған» қатесі (ERR_OPEN)**

Модульдегі T+ және T- қыстырғыштарын және түйіннің өзіндегі сым бүтіндігін тексеріңіз: термопаралар көбіне иілген жерде сынады. Термопарасыз модуль әрқашан осы кодты береді.

**Температура өсуі керек кезде төмендейді**

Термопара полярлығы шатасқан: T+ және T- қыстырғыштарындағы сымдарды ауыстырыңыз (ANSI бойынша сары плюс, қызыл минус; IEC бойынша жасыл плюс, ақ минус).

**Температура секіреді немесе бекер көрсетеді**

Қыздырғыш немесе реленің күштік тізбектерінен кедергі: термопара сымдарын күштік сымдардан алыстатыңыз, T+ және T- арасына 0.1 мкФ конденсатор қосыңыз, модульді бөлек қоректендіріңіз. MAX6675 0.25 с-та бір реттен жиі сұралмауы керек.

**«Модуль жауап бермейді» қатесі (ERR_NO_DEVICE)**

SO желісінде тек бірліктер немесе нөлдер: модуль қуатын, CS, SCK, SO және GND сымдарын және бағдарламадағы пин нөмірлерінің қосуға сәйкестігін тексеріңіз.

**MAX6675 нөлден төмен температураны көрсетпейді**

Бұл чиптің шектеуі: ол 0..1023.75 °C өлшейді. Теріс температуралар үшін MAX31855 алыңыз (-200..1350 °C).

### Өзін-өзі тексеру
Кітапханада `diagnose(Serial)` бар: тақта не көргенін басып шығарады және нені тексеру керегін айтады. Кеңес тілі: `setLanguage(LANG_RU / LANG_KK / LANG_EN)`.

### Мысалдар
`Basic`, `Thermostat`, `Diagnose`

---

## English

A simple library for **type K thermocouples** through **MAX6675** (0..1024 °C) and **MAX31855** (-200..1350 °C) modules. The temperature of a kiln, a soldering iron, a candle, an exhaust. Finds a break and a short, shows the cold junction temperature and explains what is wrong. Software SPI on any pins or hardware SPI. Only the built-in `SPI`. Made for classes and Alash Electronics kits.

### Installation
Arduino IDE: Sketch, Include Library, Manage Libraries, search **AlashThermocouple** and click Install. Or download the ZIP from GitHub: Sketch, Include Library, Add .ZIP Library.

### Boards
Arduino Uno / Nano / Mega (AVR), ESP32, ESP8266 and others with an Arduino core.

### Wiring
| MAX6675 / MAX31855 | Uno (пример) | ESP32 |
|---|---|---|
| VCC | 3.3 В или 5 В | 3.3 В |
| GND | GND | GND |
| SCK | пин 6 | любой пин |
| CS | пин 5 | любой пин |
| SO | пин 4 | любой пин |
| T+ / T- | термопара (плюс на T+) | термопара |

Pins are given in the program (software SPI) or hardware ones are used (`AlashThermocouple tc(MAX31855, CS, SPI)`). A thermocouple is polarized: ANSI yellow plus and red minus, IEC green plus and white minus. Do not run the thermocouple wires beside the heater power circuit.

### Quick start
```cpp
#include <AlashThermocouple.h>
AlashThermocouple tc(AlashThermocouple::MAX6675, 5, 6, 4);   // чип, CS, SCK, SO

void setup() { Serial.begin(115200); tc.begin(); }
void loop() {
  Serial.println(tc.temperature());   // °C, NAN при обрыве
  delay(1000);
}
```

### Features
- **`temperature()` in one line:** °C (and °F), `NAN` on a fault.
- **MAX6675 and MAX31855:** one class, the chip is a parameter.
- **Open and short:** `ERR_OPEN`, `ERR_SHORT_GND`, `ERR_SHORT_VCC` (MAX31855), `ERR_NO_DEVICE` if the module does not answer.
- **Cold junction:** `internalTemperature()` on the MAX31855, available even with a break.
- **Software or hardware SPI:** any three pins or the board SPI.
- **Cache:** the module is not polled faster than it measures (250 ms and 100 ms), `setCacheMs()`.
- **Offset:** `setOffset()` against a reference thermometer.
- **See what happens:** `diagnose(Serial)` shows the raw frame and tips (polarity, noise, power) in three languages.
- Works on **Arduino AVR, ESP32, ESP8266**.

### Method reference
| Method | What it does |
|---|---|
| `AlashThermocouple(chip, cs, clk, miso)` | Software SPI: chip MAX6675 or MAX31855 and three pins |
| `AlashThermocouple(chip, cs, SPI)` | Hardware SPI: CS only |
| `begin()` | Set up the pins; false if the module does not answer |
| `temperature() / temperatureF()` | Thermocouple temperature, NAN on error |
| `internalTemperature()` | Cold junction (MAX31855) |
| `setOffset(deg) / setCacheMs(ms)` | Offset / polling interval |
| `update() / raw()` | Read now / last raw frame |
| `error() / errorText() / setLanguage()` | Error and message in ru/kk/en |
| `diagnose(Serial)` | Self-check with tips |

### If something does not work
**A "thermocouple not connected" error (ERR_OPEN)**

Check the T+ and T- terminals on the module and the wire right at the junction: thermocouples often break at the bend. A module without a thermocouple always gives this code.

**The temperature falls when it should rise**

The thermocouple polarity is swapped: swap the wires on the T+ and T- terminals (ANSI yellow plus, red minus; IEC green plus, white minus).

**The temperature jumps or shows nonsense**

Noise from the heater or relay power circuits: move the thermocouple wires away from the power ones, add a 0.1 uF capacitor between T+ and T-, power the module separately. Poll a MAX6675 no more often than once per 0.25 s.

**A "module does not answer" error (ERR_NO_DEVICE)**

The SO line is all ones or all zeros: check the module power, the CS, SCK, SO and GND wires and that the pin numbers in the program match the wiring. On ESP32 and ESP8266 prefer general-purpose pins (not 34-39 for the CS and SCK outputs).

**MAX6675 does not show temperatures below zero**

This is a chip limit: it measures 0..1023.75 °C. For negative temperatures take the MAX31855 (-200..1350 °C).

### Self-check
The library has `diagnose(Serial)`: it prints what the board sees and tells what to check. Tips language: `setLanguage(LANG_RU / LANG_KK / LANG_EN)`.

### Examples
`Basic`, `Thermostat`, `Diagnose`

---

MIT License, Alash Electronics. https://github.com/Alash-electronics/AlashThermocouple
