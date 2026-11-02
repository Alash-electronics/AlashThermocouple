# AlashThermocouple

[Русский](#русский) | [Қазақша](#қазақша) | [English](#english)

---

## Русский

Простая библиотека для **термопар типа K** через модули **MAX6675** (0..1024 °C) и **MAX31855** (-200..1350 °C). Температура печи, паяльника, свечи, выхлопа. Находит обрыв и замыкание, показывает температуру холодного спая, объясняет, что не так. Программный SPI на любых пинах или аппаратный. Только встроенная `SPI`. Сделана для уроков и наборов Alash Electronics.

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

### Чем отличается от других библиотек
Мы изучили Adafruit MAX6675 и Adafruit MAX31855 (лицензия в файлах BSD, без SPDX) и другие библиотеки для этих чипов:
- Каждая библиотека для одного чипа; у нас один интерфейс для обоих.
- Adafruit возвращает NAN без причины, кроме `readError()` у MAX31855; у нас понятный код и текст на трёх языках.
- Нет отдельной диагностики «модуля нет вообще» (MISO в воздухе или на земле); у нас `ERR_NO_DEVICE`.
- Нет кэша: опрос чаще 0.22 с у MAX6675 отдаёт старые данные; у нас кэш по времени преобразования.
- Adafruit тянет Adafruit_BusIO; у нас только `SPI`.

### Примеры
`Basic`, `Thermostat`, `Diagnose`

### Тесты
Без железа (с AddressSanitizer) на имитации шины и микросхем, где биты сдвигаются по спаду SCK, как на настоящих: разбор кадров по таблицам даташитов (+1600, +0.25, 0, -0.25, -250 °C; внутренняя 127, 25, -55 °C), обрыв, замыкания, нет модуля, чтение по программному и аппаратному SPI (режим 0), кэш, поправка, ошибки и диагностика: `extras/test`.

Подробное руководство: [docs/WIKI.md](docs/WIKI.md).

---

## Қазақша

**MAX6675** (0..1024 °C) және **MAX31855** (-200..1350 °C) модульдері арқылы **K типті термопараларға** арналған қарапайым кітапхана. Пеш, дәнекерлегіш, шам, түтін температурасы. Үзілу мен тұйықталуды табады, суық түйін температурасын көрсетеді, не дұрыс емесін түсіндіреді. Кез келген пиндегі бағдарламалық SPI немесе аппараттық. Тек кірістірілген `SPI`.

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

### Басқа кітапханалардан айырмашылығы
Adafruit MAX6675 және Adafruit MAX31855 (файлдардағы лицензия BSD) және осы чиптерге арналған басқа кітапханаларды қарадық:
- Әр кітапхана бір чипке арналған; бізде екеуіне де бір интерфейс.
- Adafruit себепсіз NAN қайтарады; бізде түсінікті код пен үш тілде мәтін.
- «Модуль мүлде жоқ» (MISO ауада немесе жерде) жеке диагностикасы жоқ; бізде `ERR_NO_DEVICE`.
- Кэш жоқ: MAX6675-ті 0.22 с-тан жиі сұрау ескі деректер береді; бізде түрлендіру уақыты бойынша кэш.
- Adafruit Adafruit_BusIO тартады; бізде тек `SPI`.

### Мысалдар
`Basic`, `Thermostat`, `Diagnose`

### Тесттер
Құрылғысыз (AddressSanitizer-мен) бит нақты чиптердегідей SCK құлауымен жылжитын шина мен чиптер имитациясында: даташит кестелері бойынша кадр талдауы, үзілу, тұйықталулар, модуль жоқ, бағдарламалық және аппараттық SPI (0 режимі) арқылы оқу, кэш, түзету, қателер: `extras/test`.

Толық нұсқаулық: [docs/WIKI.md](docs/WIKI.md).

---

## English

A simple library for **type K thermocouples** through **MAX6675** (0..1024 °C) and **MAX31855** (-200..1350 °C) modules. The temperature of a kiln, a soldering iron, a candle, an exhaust. Finds a break and a short, shows the cold junction temperature and explains what is wrong. Software SPI on any pins or hardware SPI. Only the built-in `SPI`. Made for classes and Alash Electronics kits.

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

### How it differs from other libraries
We studied Adafruit MAX6675 and Adafruit MAX31855 (BSD license in the files, no SPDX) and other libraries for these chips:
- Each library is for one chip; ours is one interface for both.
- Adafruit returns NAN without a reason apart from `readError()` on the MAX31855; ours has a clear code and text in three languages.
- No separate diagnosis for a module that is not there at all (MISO floating or grounded); ours gives `ERR_NO_DEVICE`.
- No cache: polling a MAX6675 faster than 0.22 s returns old data; ours caches by the conversion time.
- Adafruit pulls in Adafruit_BusIO; ours needs only `SPI`.

### Examples
`Basic`, `Thermostat`, `Diagnose`

### Tests
Without hardware (with AddressSanitizer) on a simulated bus and chips where the bits shift on the falling SCK edge like on the real ones: frame decoding against the data-sheet tables (+1600, +0.25, 0, -0.25, -250 °C; internal 127, 25, -55 °C), open, shorts, no module, reading over software and hardware SPI (mode 0), cache, offset, errors and diagnostics: `extras/test`.

Full guide: [docs/WIKI.md](docs/WIKI.md).

---

MIT License, Alash Electronics.
