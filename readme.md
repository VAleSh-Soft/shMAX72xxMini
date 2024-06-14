## shMAX72xxMini - небольшая быстрая библиотека для работы с драйверами MAX7219 и MAX7221

Создана на основе библиотеки LedControl - http://wayoda.github.io/LedControl/

- [Основные возможности библиотеки:](#основные-возможности-библиотеки)
- [История версий](#история-версий)
- [Класс shMAX72xxMini](#класс-shmax72xxmini)
  - [Остальные методы класса **shMax72xxMini**](#остальные-методы-класса-shmax72xxmini)
- [Класс shMAX72xx7Segment](#класс-shmax72xx7segment)
  - [Остальные методы класса **shMAX72xx7Segment**](#остальные-методы-класса-shmax72xx7segment)
- [Подключение драйвера к Ардуино](#подключение-драйвера-к-ардуино)
  - [Ардуино UNO/Nano/Pro Mini](#ардуино-unonanopro-mini)
    - [SPI1 - ATmega328pb](#spi1---atmega328pb)
  - [ATtiny88](#attiny88)
  - [ESP8266](#esp8266)
  - [ESP32](#esp32)
  - [RP2040](#rp2040)
  - [STM32](#stm32)
- [Аддоны, используемые для работы с другими платами](#аддоны-используемые-для-работы-с-другими-платами)
  - [ATmega328pb](#atmega328pb)
  - [ATtiny88](#attiny88-1)
  - [ESP8266](#esp8266-1)
  - [ESP32](#esp32-1)
  - [RP2040](#rp2040-1)
  - [STM32](#stm32-1)

### Основные возможности библиотеки:
- управление состоянием светодиодов подключенных устройств поштучно, по строкам и по столбцам;
- управление положением изображения - поворот и отражение по горизонтали (по строкам) в случае необходимости (привет китайским светодиодным модулям);
- библиотека может использоваться для работы как со светодиодными матрицами 8х8 светодиодов, так и с семисегментными цифровыми индикаторами;
- возможность изменения частоты **SPI**-интерфейса для работы драйвера; поддержка **SPI**-транзакций, т.е. ваше устройство не будет мешать работать другим устройства, подключенным к этому же интерфейсу, но имеющим другие настройки;
- возможность выбора интерфейса **SPI** для МК, которые это поддерживают;
- возможность переназначения пинов **SPI**-интерфейса для МК, которые это поддерживают;

### История версий
Версия 1.5 - 11.06.2024
+ добавлена возможность изменения интерфейса **SPI** для вывода данных для **ESP32**, **RP2040** и **STM32**;
+ добавлен метод `void setSPI(shSPIClass *spi);`, позволяющий выбрать интерфейс **SPI** для вывода данных; метод доступен для **ESP32**, **RP2040** и **STM32**;
+ добавлена возможность изменять пины интерфейса **SPI** для подключения драйвера; чтобы узнать, какие пины могут быть назначены для того или иного интерфейса, обращайтесь к описанию своего микроконтроллера;
+ добавлен метод `void init(int8_t sck_pin, int8_t mosi_pin, int8_t miso_pin = -1);`, позволяющий задать соответствующие пины текущего интерфейса **SPI**; метод доступен для **ESP32**, **RP2040**;
+ добавлен метод `void init(int8_t spi)`, предназначенный для инициализации интерфейса на **ATmega328pb** с возможностью выбора одного из двух доступных **SPI**;
+ добавлены примеры одновременного использования двух **SPI**-интерфейсов на примере одновременного вывода двух бегущих строк; примеры написаны для **ATmega328pb**, **ESP32**, **STM32 Blue Pill** и **RP2040**;
+ метод `setSPISettings()` заменен методом `setSPIFrequensy(uint32_t frequensy)`, т.к. частота работы **SPI**-интерфейса - это единственное, что имеет смысл менять для драйвера **MAX7219/7221**;
+ доработан пример **TwoTickersForESP32.ino** для работы на **ESP32S2** и **ESP32S3**;
* мелкие исправления и доработки;

Версия 1.4 - 16.04.2024
+ добавлена поддержка **SPI**-транзакций; по умолчанию драйвер работает с частотой синхронизации **1 МГц**, с параметрами **MSBFIRST** и **SPI_MODE0**;
+ добавлен метод, позволяющий настроить параметры **SPI** для работы драйвера;
+ инициализация драйвера перенесена из конструктора в метод `init()`;
* в связи с этим обновлены примеры;
* мелкие исправления и доработки;

Версия 1.3 - 31.03.2024
* при создании экземпляра класса устройство включается теперь сразу с минимальной яркостью экрана;
+ добавлен метод `getColumn(uint8_t addr, uint8_t column)`, позволяющий получить текущую битовую маску столбца **column** в устройстве с индексом **addr**; учитываются заданные для матрицы параметры поворота и отражения по строкам; данные берутся не непосредственно из модуля, а из его программного буфера;
+ добавлен метод `getRow(uint8_t addr, uint8_t row)`, позволяющий получить текущую битовую маску  строки **row** в устройстве с индексом **addr**; учитываются заданные для матрицы параметры поворота и отражения по строкам; данные берутся не непосредственно из модуля, а из его программного буфера;
+ добавлен метод `getLedStat(uint8_t addr, uint8_t row, uint8_t column)`, позволяющий получить текущее состояние светодиода в строке **row** столбца **column** в устройстве с индексом **addr**; учитываются заданные для матрицы параметры поворота и отражения по строкам; данные берутся не непосредственно из модуля, а из его программного буфера;
+ добавлен метод `setBrightnessForAllDevices(uint8_t intensity)`, позволяющий установить яркость свечения светодиодов для всех устройств сразу;
* исправлена ошибка работы с `PROGMEM` в **esp32**;
* обновлены примеры;

Версия 1.2 - 29.07.2022
+ добавлен класс **shMAX72xx7Segment** с парой методов, облегчающих работу с семисегментными индикаторами;

Версия 1.0 - 26.07.2022
* заменены типы используемых данных для уменьшения потребления памяти;
* увеличено максимальное количество устройств в каскаде до 127 шт.; с большим числом корректной работы не будет;
* работа с драйвером переведена на аппаратный **SPI**, что хорошо сказалось на производительности библиотеки;
* убраны одни и добавлены другие методы; в том числе:
+ добавлена возможность поворота изображения и зеркального отражения его по горизонтали (т.е. по строке) при необходимости;
+ добавлена возможность обновления изображения на всех устройствах сразу;


### Класс shMAX72xxMini

При создании объекта **shMax72xxMini** нужно сразу указать номер пина Ардуино для подключения вывода **CS** (или **LOAD**) модуля и количество используемых устройств в модуле:

`MAX72xxMini<10, 8> disp;`

Здесь:
- 10 - номер   пина Ардуино для подключения вывода **CS** (или **LOAD**) модуля;
- 8 - количество используемых в модуле устройств, может быть установлено в интервале 1-127;

Метод

`void init()` выполняет инициализацию драйвера.

Если используются светодиодные матрицы 8х8 светодиодов, то может понадобиться поворот изображения и (или) зеркальное отражение его по горизонтали (т.е. по строкам). Для этого используются методы:

`disp.setDirection(2);`, где **2** - угол поворота изображения, может быть задан в интервале 0-3;

`disp.setFlip(true);`, где **true** включает отражение, **false** - отключает отражение изображения;

Оба метода вносят настройки для всех устройств модуля сразу.

#### Остальные методы класса **shMax72xxMini**

`void init(uint8_t spi = SPI_SPI0)` -  - инициализация драйвера для **ATmega328pb** с возможностью выбора интерфейса для вывода данных; здесь:
- `spi` - индекс интерфейса; возможные варианты: **SPI_SPI0** - интерфейс **SPI**, **SPI_SPI1** - интерфейс **SPI1**.

Метод доступен для МК **ATmega328pb** при использовании аддона **MiniCore** (см. [ATmega328pb](#atmega328pb)).

Для МК, позволяющих переназначать пины **SPI**-интерфейсов доступен метод:

`void init(int8_t sck_pin, int8_t mosi_pin, int8_t miso_pin = -1)` - инициализация драйвера с переназначением пинов **SPI**-интерфейса для подключения устройства; здесь:
- `sck_pin` - пин для подключения вывода **SLK** устройства;
- `mosi_pin` - пин для подключения вывода **DIN** устройства;
- `miso_pin` - пин **MISO** выбранного интерфейса; этот параметр обязателен при использовании **ESP32** в связи с особенностями реализации инициализации **SPI**-интерфейсов на этих МК; 

Для определения, какие пины могут быть назначены для того или иного **SPI**-интерфейса, обращайтесь к описанию вашего МК.

`void setSPI(shSPIClass *spi)` - метод задает **SPI**-интерфейс для вывода данных; метод доступен для МК, имеющих более одного **SPI**-интерфейса (**ESP32**, **RP2040**, **STM32**); здесь:
- `*spi` - указатель на требуемый **SPI**-интерфейс;

`void setSPIFrequency(uint32_t frequensy)` - настройка частоты **SPI** для работы драйвера; здесь:
- `frequensy` - частота работы **SPI**, Гц; 

`void shutdownDevice(uint8_t addr, bool off_device)` - перевод в режим сна или вывод из него устройства с индексом **addr**; здесь:
- `off_off_device` - определяет действие: при **true** переводит устройство в режим сна (при этом информация в его регистрах сброшена не будет), при **false** - включает его;

`void shutdownAllDevices(bool off_device)` - то же самое, но для всех устройств в каскаде сразу;

`uint8_t getDeviceCount()` - получение количества устройств в модуле; (нужно понимать, что пересчитать реальное количество устройств невозможно, поэтому будет возвращено значение, использованное при создании экземпляра класса);

`void setScanLimit(uint8_t addr, uint8_t limit)` - установка лимита обработки строк устройством; здесь:
- `addr` - индекс устройства в каскаде;
- `limit` - значение, допустимый интервал 1..8.

Если задать лимит меньше 8, то устройство будет отображать информацию только на заданном количестве строк. Это может быть полезно, если к устройству подключено, например, всего 4 семисегментных индикатора вместо возможных 8; (нужно понимать, что эта настройка вносится в регистры драйвера и не учитывает заданный угол поворота изображения, т.е. будут работать только светодиоды, подключенные к соответствующим выводам микросхемы драйвера).

`void setBrightness(uint8_t addr, uint8_t intensity)` - установка яркости свечения светодиодов; здесь:
- `addr` - индекс устройства в каскаде;
- `intensity` - значение яркости; может быть задано в интервале 0-15, при этом значение 0 не отключает светодиоды полностью - они будут светиться с минимальной яркостью.

`void setBrightnessForAllDevices(uint8_t intensity)` - то же самое для всех устройств каскада сразу.

`void clearDevice(uint8_t addr, bool upd = false)` - очистить изображение на устройстве; здесь:
- `addr` - индекс устройства в каскаде;
- `upd` - признак обновления устройства; при **upd = false**, то будет очищен только программный буфер устройства, само изображение будет очищено только после вызова метода `update()`.

`void clearAllDevices(bool upd = false)` - то же самое, но для всей устройств каскада сразу.

`void setLed(uint8_t addr, uint8_t row, uint8_t column, boolean state, bool upd = false)` - установить состояние одиночного светодиода на устройстве; здесь:
- `addr` - индекс устройства в каскаде;
- `row` - строка (координата **Y**);
- `column` - столбец, (координата **X**); 
- `state` - устанавливаемое состояние светодиода - включен/выключен; 
- `upd` - признак обновления устройства; при **upd = false**, то будет очищен только программный буфер устройства, само изображение будет обновлено только после вызова метода `update()`.

Метод учитывает заданные поворот и отражение изображения.

`uint8_t getLedStat(uint8_t addr, uint8_t row, uint8_t column)` - получить состояние одиночного светодиода на устройстве; здесь: 
- `addr` - индекс устройства в каскаде;
- `row` - строка (координата **Y**);
- `column` - столбец, (координата **X**); 

Метод учитывает заданные поворот и отражение изображения.

`void setRow(uint8_t addr, uint8_t row, uint8_t value, bool upd = false)` - установка строки на устройстве; здесь:
- `addr` - индекс устройства в каскаде;
- `row` - строка (0..7);
- `value` - битовая маска строки; состояние каждого светодиода строки определяется значением соответствующего бита в параметре **value**: 1 - включен, 0 - выключен;
- `upd` - признак обновления устройства; при **upd = false**, то будет очищен только программный буфер устройства, само изображение будет обновлено только после вызова метода `update()`.

Метод учитывает заданные поворот и отражение изображения.

`uint8_t getRow(uint8_t addr, uint8_t row)` - получение битовой маски строки; здесь:
- `addr` - индекс устройства в каскаде;
- `row` - строка (0..7);

Метод учитывает заданные поворот и отражение изображения.

`void setColumn(uint8_t addr, uint8_t column, uint8_t value, bool upd = false)` - установка столбца на устройстве; здесь:
- `addr` - индекс устройства в каскаде;
- `column` - столбец (0..7);
- `value` - битовая маска строки; состояние каждого светодиода строки определяется значением соответствующего бита в параметре **value**: 1 - включен, 0 - выключен;
- `upd` - признак обновления устройства; при **upd = false**, то будет очищен только программный буфер устройства, само изображение будет обновлено только после вызова метода `update()`.

Метод учитывает заданные поворот и отражение изображения.

`uint8_t getColumn(uint8_t addr, uint8_t column)` - получение битовой маски столбца; здесь:
- `addr` - индекс устройства в каскаде;
- `column` - столбец (0..7);

Метод учитывает заданные поворот и отражение изображения.

`void update()` - обновление изображения (вывод содержимого программного буфера) на всех устройствах модуля;

### Класс shMAX72xx7Segment

Небольшое расширение класса **shMAX72xxMini** для работы с семисегментными индикаторами.

Каждый драйвер может управлять индикаторами до восьми цифр. Управление ведется по строкам, каждая строка - одна цифра семисегментного индикатора.

Работа с классом аналогична работе с **shMAX72xxMini** с минимальными отличиями.

При создании объекта кроме пина для подключения вывода CS и количества устройств в каскаде необходимо указывать и количество используемых разрядов цифровых индикаторов (сколько цифр будет использовано в работе):

`MAX72xxMini<10, 1, 8> disp;`

Здесь:
- 10 - номер   пина Ардуино для подключения вывода **CS** (или **LOAD**) модуля;
- 1 - количество используемых в модуле устройств, может быть установлено в интервале 1-127;
- 8 - количество используемых цифр, может быть установлено в интервале 1-1016 (конечно, не более, чем количество устройств в модуле, умноженное на 8);

#### Остальные методы класса **shMAX72xx7Segment**

`uint8_t getNumDigits()` - возвращает количество используемых в модуле цифр (точнее, количество, указанное при создании объекта класса);

`uint8_t encodeDigit(uint8_t digit)` - получение битовой маски для вывода символа на индикатор; здесь:
- `digit` - число, маску которого нужно получить, может задаваться в интервале 0-15; позволяет получать битовые маски цифр 0-9 и символов A, b, C, d и E;

`void setChar(uint16_t index, uint8_t value, bool showDot = false, bool upd = false)` - вывод битовой маски на цифровой индикатор; здесь:
- `index` - индекс разряда индикатора для вывода, начиная с нуля; **
- `value` - битовая маска символа, может быть получена с помощью метода `encodeDigit()` или создана "вручную"; например, **value = 0x63** (т.е. **0b01100011**) выведет на индикатор значок градуса Цельсия; 
- `showDot` - включать или нет десятичную точку для этого разряда; 
- `upd` - признак обновления устройства; при **upd = false**, то будет очищен только программный буфер устройства, само изображение будет обновлено только после вызова метода `update()`.

В остальном работа с классом не отличается от работы с классом **shMAX72xxMini**.

### Подключение драйвера к Ардуино

На данный момент библиотека тестировалась в работе на **ATmega168p/328p/328pb**, **esp8266**, **esp32**, **esp32c3**, **esp32s2**, **esp32s3**, **rp2040** (**Zero** и **Pico**), **stm32f103c8t6** (**Blue Pill**). 

Работа с интерфейсом, отличным от **SPI0**, тестировалась на **ATmega328pb**, **esp32**, **esp32s2**, **esp32s3**, **rp2040**, **stm32f103** (**Blue Pill**). 

Переназначение пинов **SPI**-интерфейса протестировано на **esp32**, **esp32c3**, **esp32s2**, **esp32s3**, **rp2040**.

#### Ардуино UNO/Nano/Pro Mini

Для работы драйвера нужно подключить его к пинам **SPI** Ардуино:
- **DataIn** (**DIN**) - к пину **MOSI** (для Ардуино Uno/NANO/Pro Mini это пин **D11**);
- **CLK** - к пину **CLK** (для Ардуино Uno/NANO/Pro Mini это пин **D13**);
- **CS**(**LOAD**) - "выбор устройства", задается при инициализации - к любому цифровому пину на ваше усмотрение;

##### SPI1 - ATmega328pb

Для работы драйвера на интерфейсе **SPI1** (с выбором этого интерфейса, см. описание метода `init(uint8_t spi = SPI_SPI0)`) нужно подключить его к cоответствующим пинам:
- **DataIn** (**DIN**) - к пину **MOSI1** (**A7**);
- **CLK** - к пину **CLK1** (**A1**);
- **CS**(**LOAD**) - "выбор устройства", задается при инициализации - к любому цифровому пину на ваше усмотрение;

#### ATtiny88

Для работы драйвера нужно подключить его к пинам **SPI** Ардуино:
- **DataIn** (**DIN**) - к пину **MOSI** (**11** или **PB3**);
- **CLK** - к пину **CLK** (**13** или **PB5**);
- **CS**(**LOAD**) - "выбор устройства", задается при инициализации - к любому цифровому пину на ваше усмотрение;

#### ESP8266

Протестировано подключение по умолчанию.

драйвер подключается к пинам **HSPI** модуля, а именно:
- **DataIn** (**DIN**) - к пину **MOSI** (**GPIO13**, он же **D7** для NodeMCU и т.п.);
- **CLK** - к пину **CLK** (**GPIO14**, он же **D5** для NodeMCU и т.п.);
- **CS**(**LOAD**) - к **GPIO15** (он же **D8** для NodeMCU и т.п.);

#### ESP32

Протестировано подключение по умолчанию, без переназначения пинов.

Для работы с **ESP32** драйвер подключается к пинам:
- **VSPI**
  - **DataIn** (**DIN**) - к пину **VSPI_MOSI** (**GPIO23**);
  - **CLK** - к пину **VSPI_SCK** (**GPIO18**);
  - **CS**(**LOAD**) - к пину **GPIO5**;
- **HSPI** (с назначением этого интерфейса, см. описание метода `setSPI()`)
  - **DataIn** (**DIN**) - к пину **HSPI_MOSI** (**GPIO13**);
  - **CLK** - к пину **HSPI_SCK** (**GPIO14**);
  - **CS**(**LOAD**) - к пину **GPIO15**;

-------------------------------------------------

Для работы с **ESP32C3** драйвер подключается к пинам **FSPI** модуля, а именно:
- **DataIn** (**DIN**) - к пину **MOSI** (**GPIO6**);
- **CLK** - к пину **SCK** (**GPIO4**);
- **CS**(**LOAD**) - к пину **GPIO7**;

Интерфейс **HSPI** для этого МК в среде **Arduino IDE** недоступен.

-------------------------------------------------

Для работы с **ESP32S3** и **ESP32S2** драйвер подключается к пинам:
- **FSPI**
  - **DataIn** (**DIN**) - к пину **MOSI** (**GPI11**);
  - **CLK** - к пину **SCK** (**GPI12**);
  - **CS**(**LOAD**) - к пину **GPIO9**; **ВАЖНО** - это не должен быть пин **GPI10**;
- **HSPI** - этот интерфейс не имеет пинов по умолчанию, поэтому работа драйвера тестировалась на следующих пинах:
  - **DataIn** (**DIN**) - к пину **GPI06**;
  - **CLK** - к пину **GPI07**;
  - **CS**(**LOAD**) - к пину **GPIO8**;

-------------------------------------------------

#### RP2040

Протестировано подключение по умолчанию, без переназначения пинов.

Для работы с **RP2040** драйвер подключается к пинам:
- **SPI0**
  - **DataIn** (**DIN**) - к пину **TX0** (**GPIO3**);
  - **CLK** - к пину **SCL0** (**GPIO2**);
  - **CS**(**LOAD**) - к пину **CS0** (**GPIO5**);
- **SPI1**
  - **DataIn** (**DIN**) - к пину **TX1** (**GPI15**);
  - **CLK** - к пину **SCL1** (**GPI14**);
  - **CS**(**LOAD**) - к пину **CS1** (**GPI13**);

#### STM32

Протестировано подключение по умолчанию. Библиотека тестировалась на **STM32F103C8T6** (**Blue Pill**)

Для работы с **Blue Pill** драйвер подключается к пинам:
- **SPI1**
  - **DataIn** (**DIN**) - к пину **MOSI1** (**PA7**);
  - **CLK** - к пину **SCK1** (**PA5**);
  - **CS**(**LOAD**) - к пину **NSS1** (**PA4**);
- **SPI2**
  - **DataIn** (**DIN**) - к пину **MOSI1** (**PB15**);
  - **CLK** - к пину **SCK1** (**PB13**);
  - **CS**(**LOAD**) - к пину **NSS1** (**PB12**);
  
Для **Blue Pill** при использовании аддона **stm32duino** возможно переназначение пинов для **SPI1** (но не средствами библиотеки):
  - **DataIn** (**DIN**) - к пину **MOSI1** (**PB5**);
  - **CLK** - к пину **SCK1** (**PB3**);

### Аддоны, используемые для работы с другими платами

Ниже представлены аддоны, не входящие в стандартную поставку **Arduino IDE**, которые использовались для проверки работы библиотеки со сторонними платами. Инструкцию по установке аддонов смотри в документации к **Arduino IDE.**

#### ATmega328pb

Для работы с МК  **ATmega3288pb** используется аддон **MiniCore by MCUdude**, ссылка для установки:
- https://mcudude.github.io/MiniCore/package_MCUdude_MiniCore_index.json

#### ATtiny88
Для работы с МК  **ATtiny88** используется аддон **ATTinyCore by Spence Konde**, ссылка для установки:
- http://drazzy.com/package_drazzy.com_index.json

#### ESP8266

Для работы с **ESP8266** используется аддон **esp8266 by ESP8266 Community**, ссылка для установки:
- http://arduino.esp8266.com/stable/package_esp8266com_index.json

Если не знаете, какую плату выбрать, выбирайте **NodeMCU 1.0 (ESP-12E Module)**.

#### ESP32

Для работы с **ESP32** используется аддон **esp32 by Espressif Systems**, ссылка для установки:
- https://dl.espressif.com/dl/package_esp32_index.json

Если не знаете, какую плату выбрать, выбирайте **ESP32 Dev Module** (или **ESP32C3 Dev Module**, **ESP32S3 Dev Module**, **ESP32S2 Dev Module** - в зависимости от того, какой процессор используется на вашей плате).

#### RP2040

Для работы с **RP2040** используется аддон **Raspberry Pi Pico/RP2040**, ссылка для установки:
- https://github.com/earlephilhower/arduino-pico/releases/download/global/package_rp2040_index.json

#### STM32

Для работы с **STM32** можно использовать как аддон **STM32 MCU based boards (stm32duino)**, ссылка для установки:
- https://github.com/stm32duino/BoardManagerFiles/raw/main/package_stmicroelectronics_index.json

так и аддон **Arduino_STM32 by Roger Clark**, ссылка на **github.com**:
- https://github.com/rogerclarkmelbourne/Arduino_STM3

Инструкция по установке:
- https://github.com/rogerclarkmelbourne/Arduino_STM32/wiki/Installation

<hr>

Если возникнут вопросы, пишите на valesh-soft@yandex.ru 
