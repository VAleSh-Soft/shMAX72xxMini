## shMAX72xxMini - небольшая быстрая библиотека для работы с драйверами MAX7219 и MAX7221

Создана на основе библиотеки LedControl - http://wayoda.github.io/LedControl/

- [Основные возможности библиотеки:](#основные-возможности-библиотеки)
- [История версий](#история-версий)
- [Подключение матрицы к Ардуино](#подключение-матрицы-к-ардуино)
- [Класс shMAX72xxMini](#класс-shmax72xxmini)
- [Класс shMAX72xx7Segment](#класс-shmax72xx7segment)

### Основные возможности библиотеки:
- управление состоянием светодиодов подключенных устройств поштучно, по строкам и по столбцам;
- управление положением изображения - поворот и отражение по горизонтали (по строкам) в случае необходимости (привет китайским светодиодным модулям);
- библиотека может использоваться для работы как со светодиодными матрицами 8х8 светодиодов, так и с семисегментными цифровыми индикаторами;
- возможность изменения параметров **SPI** для работы драйвера;

### История версий
Версия 1.4 - 15.04.2024
+ добавлена поддержка SPI-транзакций; по умолчанию драйвер работает с частотой синхронизации 1 МГц, с параметрами **MSBFIRST** и **SPI_MODE0**;
+ добавлен метод, позволяющий настроить параметры **SPI** для работы драйвера;
+ инициализация матрицы перенесена из конструктора в метод `init()`;
* мелкие исправления и доработки;

Версия 1.3 - 31.03.2024
* при создании экземпляра класса питание на устройства подается теперь сразу с минимальной яркостью экрана;
+ добавлен метод `getColumn(uint8_t addr, uint8_t column)`, позволяющий получить текущую битовую маску столбца **column** в устройстве с индексом **addr**; учитываются заданные для матрицы параметры поворота и отражения по строкам; данные берутся не непосредственно из модуля, а из его программного буфера;
+ добавлен метод `getRow(uint8_t addr, uint8_t row)`, позволяющий получить текущую битовую маску  строки **row** в устройстве с индексом **addr**; учитываются заданные для матрицы параметры поворота и отражения по строкам; данные берутся не непосредственно из модуля, а из его программного буфера;
+ добавлен метод `getLedStat(uint8_t addr, uint8_t row, uint8_t column)`, позволяющий получить текущую состояние светодиода в строке **row** столбца **column** в устройстве с индексом **addr**; учитываются заданные для матрицы параметры поворота и отражения по строкам; данные берутся не непосредственно из модуля, а из его программного буфера;
+ добавлен метод `setBrightnessForAllDevices(uint8_t intensity)`, позволяющий установить яркость свечения светодиодов для всех устройств сразу;
* исправлена ошибка работы с `PROGMEM` в **esp32**;
* обновлены примеры;

Версия 1.2 - 29.07.2022
+ добавлен класс **shMAX72xx7Segment** с парой методов, облегчающих работу с семисегментными индикаторами;

Версия 1.0 - 26.07.2022
* заменены типы используемых данных для уменьшения потребления памяти;
* увеличено максимальное количество устройств в каскаде до 127 шт.; с большим числом корректной работы не будет;
* работа с драйвером переведена на аппаратный SPI, что хорошо сказалось на производительности библиотеки;
* убраны одни и добавлены другие методы; в том числе:
+ добавлена возможность поворота изображения и зеркального отражения его по горизонтали (т.е. по строке) при необходимости;
+ добавлена возможность обновления изображения на всех устройствах сразу;

### Подключение матрицы к Ардуино

Для работы матрицы нужно подключить ее к пинам SPI Ардуино:
- **DataIn** (**DIN**) - к пину **MOSI** (для Ардуино Uno/NANO/Pro Mini это пин **D11**);
- **CLK** - к пину **CLK** (для Ардуино Uno/NANO/Pro Mini это пин **D13**);
- **CS**(**LOAD**) - "выбор устройства", задается при инициализации - к любому цифровому пину на ваше усмотрение;

Библиотека поддерживает работу с **ESP8266**, в этом случае матрица подключается к пинам **HSPI** модуля, а именно:
- **DataIn** (**DIN**) - к пину **MOSI** (**GPIO13**, он же **D7** для NodeMCU и т.п.);
- **CLK** - к пину **CLK** (**GPIO14**, он же **D5** для NodeMCU и т.п.);
- **CS**(**LOAD**) - к **GPIO15** (он же **D8** для NodeMCU и т.п.);

### Класс shMAX72xxMini

При создании объекта **shMax72xxMini** нужно сразу указать номер пина Ардуино для подключения вывода **CS** (или **LOAD**) модуля и количество используемых устройств в модуле:

`MAX72xxMini<10, 8> disp;`

Здесь:
- 10 - номер   пина Ардуино для подключения вывода **CS** (или **LOAD**) модуля;
- 8 - количество используемых в модуле устройств, может быть установлено в интервале 1-127;

Метод

`void init()` выполняет инициализацию матрицы.

Если используются светодиодные матрицы 8х8 светодиодов, то может понадобиться поворот изображения и (или) зеркальное отражение его по горизонтали (т.е. по строкам). Для этого используются методы:

`disp.setDirection(2);`, где **2** - угол поворота изображения, может быть задан в интервале 0-3;

`disp.setFlip(true);`, где **true** включает отражение, **false** - отключает отражение изображения;

Оба метода вносят настройки для всех устройств модуля сразу.

Остальные методы класса **shMax72xxMini**

`void setSPISettings(uint32_t clock, uint8_t bitOrder, uint8_t dataMode)` - настройка параметров **SPI** для работы драйвера; **clock** - частота синхронизации **SPI**, Гц; **bitOrder** - порядок следования бит при передаче, возможные варианты - **MSBFIRST**, **LSBFIRST**; **dataMode** - режим передачи данных, возможные варианты - **SPI_MODE0**, **SPI_MODE1**, **SPI_MODE2**, **SPI_MODE3**;

`void shutdownDevice(uint8_t addr, bool off_device)` - перевод в режим сна или вывод из него устройства с индексом **addr**; параметр **off_off_device** определяет действие: при **true** переводит устройство в режим сна (при этом информация в его регистрах сброшена не будет), при **false** - включает его;

`void shutdownAllDevices(bool off_device)` - то же самое, но для всех устройств в каскаде сразу;

`uint8_t getDeviceCount()` - получение количества устройств в модуле; (нужно понимать, что пересчитать реальное количество устройств невозможно, поэтому будет возвращено значение, использованное при создании экземпляра класса);

`void setScanLimit(uint8_t addr, uint8_t limit)` - установка лимита обработки строк устройством с индексом **addr**; значение **limit** может быть задано в интервале 1-8.

Если задать лимит меньше 8, то устройство будет отображать информацию только на заданном количестве строк. Это может быть полезно, если к устройству подключено, например, всего 4 семисегментных индикатора вместо возможных 8; (нужно понимать, что эта настройка вносится в регистры драйвера и не учитывает заданный угол поворота изображения, т.е. будут работать только светодиоды, подключенные к соответствующим выводам микросхемы драйвера).

`void setBrightness(uint8_t addr, uint8_t intensity)` - установка яркости свечения светодиодов устройства с индексом **addr**; значение **intensity** может быть задано в интервале 0-15, при этом значение 0 не отключает светодиоды полностью - они будут светиться с минимальной яркостью.

`void setBrightnessForAllDevices(uint8_t intensity)` - то же самое для всех устройств каскада сразу.

`void clearDevice(uint8_t addr, bool upd = false)` - очистить изображение на устройстве с индексом **addr**; если параметр **upd = false**, то будет очищен только программный буфер устройства, само изображение будет сброшено только после вызова метода `update()`.

`void clearAllDevices(bool upd = false)` - очистить изображение на всех устройствах; если параметр **upd = false**, то будет очищен только программный буфер устройств, само изображение будет сброшено только после вызова метода `update()`.

`void setLed(uint8_t addr, uint8_t row, uint8_t column, boolean state, bool upd = false)` - установить состояние одиночного светодиода на устройстве с индексом **addr** с координатами **row** (строка, координата **Y**) и **column** (столбец, координата **X**); **state** - устанавливаемое состояние светодиода - включен/выключен; метод учитывает заданные поворот и отражение изображения; если параметр **upd = false**, то изменение будет внесено только в программный буфер устройства, само изображение будет обновлено только после вызова метода `update()`.

`uint8_t getLedStat(uint8_t addr, uint8_t row, uint8_t column)` - получить состояние одиночного светодиода на устройстве с индексом **addr** с координатами **row** (строка, координата **Y**) и **column** (столбец, координата **X**); метод учитывает заданные поворот и отражение изображения;

`void setRow(uint8_t addr, uint8_t row, uint8_t value, bool upd = false)` - установка на устройстве с индексом **addr** строки с индексом **row**; строка определяется значением **value** - состояние каждого светодиода строки определяется значением соответствующего бита в параметре **value**: 1 - включен, 0 - выключен; метод учитывает заданные поворот и отражение изображения; если параметр **upd = false**, то изменение будет внесено только в программный буфер устройства, само изображение будет обновлено только после вызова метода `update()`.

`uint8_t getRow(uint8_t addr, uint8_t row)` - получение битовой маски строки **row** на устройстве с индексом **addr**; метод учитывает заданные поворот и отражение изображения;

`void setColumn(uint8_t addr, uint8_t column, uint8_t value, bool upd = false)` - установка на устройстве с индексом **addr** столбца с индексом **column**; столбец определяется значением **value** - состояние каждого светодиода столбца определяется значением соответствующего бита в параметре **value**: 1 - включен, 0 - выключен; метод учитывает заданные поворот и отражение изображения; если параметр **upd = false**, то изменение будет внесено только в программный буфер устройства, само изображение будет обновлено только после вызова метода `update()`.

`uint8_t getColumn(uint8_t addr, uint8_t column)` - получение битовой маски столбца **column** на устройстве с индексом **addr**; метод учитывает заданные поворот и отражение изображения;

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

Остальные методы класса **shMAX72xx7Segment**

`uint8_t getNumDigits()` - возвращает количество используемых в модуле цифр (точнее, количество, указанное при создании объекта класса);

`uint8_t encodeDigit(uint8_t digit)` - получение битовой маски для вывода символа на индикатор; **digit** - число, маску которого нужно получить, может задаваться в интервале 0-15; позволяет получать битовые маски цифр 0-9 и символов A, b, C, d и E;

`void setChar(uint16_t index, uint8_t value, bool showDot = false, bool upd = false)` - вывод битовой маски на цифровой индикатор; **index** - индекс индикатора для вывода, начиная с нуля; **value** - битовая маска символа, может быть получена с помощью метода **encodeDigit** или созданная "вручную"; например, **value = 0x63** (или 0b01100011) выведет на индикатор значок градуса Цельсия; **showDot** - включать или нет десятичную точку для этого разряда; если параметр **upd = false**, то изменение будет внесено только в программный буфер устройства, само изображение будет обновлено только после вызова метода `update()`.

В остальном работа с классом не отличается от работы с классом **shMAX72xxMini**.

<hr>

Если возникнут вопросы, пишите на valesh-soft@yandex.ru 
