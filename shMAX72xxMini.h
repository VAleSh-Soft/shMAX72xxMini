#pragma once

// ===================================================

// флаг использования МК ATmega 328PB, имеющим на борту два
// интерфейса SPI
#if defined(MINICORE) && defined(__AVR_ATmega328PB__)
#define MINICORE_AVR_ATMEGA328PB 1
#else
#define MINICORE_AVR_ATMEGA328PB 0
#endif

// флаг доступности второго и более интерфейса SPI
#if defined(ARDUINO_ARCH_RP2040) || defined(ARDUINO_ARCH_ESP32)
#define SPI1_AVAILABLE 1
#else
#define SPI1_AVAILABLE 0
#endif

// флаг доступности переназначения пинов SPI
#if defined(ARDUINO_ARCH_RP2040) || defined(ARDUINO_ARCH_ESP32) || \
    defined(ARDUINO_ARCH_ESP8266)
#define CUSTOM_SPI_PINS_AVAILABLE 1
#else
#define CUSTOM_SPI_PINS_AVAILABLE 0
#endif

// ===================================================

#if defined(ARDUINO_ARCH_ESP32)
#include <pgmspace.h>
#else
#include <avr/pgmspace.h>
#endif
#include <Arduino.h>
#if MINICORE_AVR_ATMEGA328PB
#include <SPI1.h>
#endif
#include <SPI.h>

// коды операций для MAX7221 и MAX7219
#define OP_NOOP 0         // пустая команда, ничего не делать
#define OP_DIGIT0 1       // первая строка
#define OP_DIGIT1 2       // вторая строка
#define OP_DIGIT2 3       // третья строка
#define OP_DIGIT3 4       // четвертая строка
#define OP_DIGIT4 5       // пятая строка
#define OP_DIGIT5 6       // шестая строка
#define OP_DIGIT6 7       // седьмая строка
#define OP_DIGIT7 8       // восьмая строка
#define OP_DECODEMODE 9   // режим декодирования; 1 - включен, 0 - отключен
#define OP_INTENSITY 10   // яркость; 0-15
#define OP_SCANLIMIT 11   // ограничение на работу со строками; 0-7
#define OP_SHUTDOWN 12    // отключение устройства; 0 - отключено, 1 - включено
#define OP_DISPLAYTEST 15 // тест дисплея, зажигает все сегменты/светодиоды

// ==== shMAX72xxMini ================================

#if MINICORE_AVR_ATMEGA328PB

constexpr uint8_t PB_SPI0 = 0;
constexpr uint8_t PB_SPI1 = 1;

#else

SPISettings spi_settings(1000000ul, MSBFIRST, SPI_MODE0);

#endif

#if defined(ARDUINO_ARCH_RP2040)
typedef SPIClassRP2040 shSPIClass;
#else
typedef SPIClass shSPIClass;
#endif

/**
 * @brief конструктор объекта
 *
 * @tparam csPin номер пина для подключения вывода CS
 * @tparam numDevices количество устройств в каскаде (1..127)
 */
template <uint8_t csPin, uint8_t numDevices>
class shMAX72xxMini
{
private:
  // массив байт для отправки в устройства
  uint8_t spidata[numDevices * 2];
  // массив состояния всех светодиодов в устройствах
  uint8_t status[numDevices * 8];

  bool flip = false;  // отразить изображение
  uint8_t direct = 0; // поворот изображения, 0-3

  // SPI интерфейс для вывода данных
#if MINICORE_AVR_ATMEGA328PB
  SPI1Class *_spi1 = NULL;
  SPIClass *_spi = NULL;
  bool is_spi1 = false;

  // настройки SPI-интерфейса
  uint32_t spi_clock = 1000000ul;
  uint8_t spi_bit_order = MSBFIRST;
  uint8_t spi_data_mode = SPI_MODE0;
#else
  shSPIClass *_spi = &SPI;
#endif

  // первоначальная инициализация матрицы
  void _init();

  // первоначальная инициализация SPI
#if MINICORE_AVR_ATMEGA328PB
  void start(uint8_t spi);
#else
  void start();
#endif
#if defined(ARDUINO_ARCH_ESP32)
  void start(int8_t sck_pin, int8_t mosi_pin, int8_t miso_pin);
#endif

  // отправка данных через SPI
  void transfer_data();

  // отправка команды по конкретному адресу
  void spiTransfer(uint8_t addr, uint8_t opcode, uint8_t data);

  // отправка команды на все устройства
  void spiTransfer(uint8_t opcode, uint8_t data);

  // изменение порядка следования битов в байте
  uint8_t reverseByte(uint8_t b);

  // отрисовка столбца
  void _setColumn(uint8_t addr, uint8_t col, uint8_t value, bool upd);

  // отрисовка строки
  void _setRow(uint8_t addr, uint8_t row, uint8_t value, bool upd);

  // отрисовка одиночного светодиода
  void _setLed(uint8_t addr, uint8_t row, uint8_t column, boolean state, bool upd);

  bool _getLedState(uint8_t addr, uint8_t row, uint8_t column);

  // обновление одного устройства; возможно обновление как только буфера, так и изображения на матрице; при этом возможно стирание буфера (и матрицы, соответсвенно)
  void _update(uint8_t addr, bool clear = false, bool transfer = true);

public:
  // Подключение устройства к Ардуино Uno/Nano/Pro mini:
  // DataIn - к пину D11
  // CLK    - к пину D13
  // CS     - к пину csPin (обычно это пин D10)
  // numDevices - количество устройств в каскаде
  shMAX72xxMini() {}

#if CUSTOM_SPI_PINS_AVAILABLE
  /**
   * @brief инициализация устройства
   *
   * @param sck_pin пин для подключения CLK
   * @param mosi_pin пин для подключения DIN (DataIn)
   * @param miso_pin пин MISO для текущего интерфейса
   */
  void init(int8_t sck_pin, int8_t mosi_pin, int8_t miso_pin = -1);
#endif

#if MINICORE_AVR_ATMEGA328PB

  /**
   * @brief инициализация устройства
   *
   * @param spi выбор интерфейса - SPI или SPI1
   */
  void init(uint8_t spi = PB_SPI0);

#else

  /**
   * @brief инициализация устройства
   *
   */
  void init();

#endif

#if SPI1_AVAILABLE
  void setSPI(shSPIClass *spi);
#endif

  /**
   * @brief настройка параметров SPI
   *
   * @param clock частота синхронизации SPI, Гц
   * @param bitOrder порядок следования бит; возможные варианты - MSBFIRST и LSBFIRST
   * @param dataMode режим работы интерфейса; возможные варианты - SPI_MODE0, SPI_MODE1, SPI_MODE2, SPI_MODE3
   */
  void setSPISettings(uint32_t clock, uint8_t bitOrder, uint8_t dataMode);

  /**
   * @brief отключить устройство
   *
   * @param addr индекс устройства в каскаде, начиная с нуля
   * @param off_device true - отключить устройство, false - включить устройство
   */
  void shutdownDevice(uint8_t addr, bool off_device);

  /**
   * @brief отключить все устройства в каскаде
   *
   * @param off_device true - отключить устройства, false - включить устройства
   */
  void shutdownAllDevices(bool off_device);

  /**
   * @brief установить угол поворота изображения на всех устройствах
   *
   * @param dir угол поворота изображения, 0..3
   */
  void setDirection(uint8_t dir);

  /**
   * @brief включить отражение изображения по горизонтали (по строкам) на всех устройствах
   *
   * @param toFlip true - включить отражение, false - отключить отражение
   */
  void setFlip(bool toFlip);

  /**
   * @brief получить количество устройств в каскаде
   *
   * @return uint8_t
   */
  uint8_t getDeviceCount();

  /**
   * @brief установить предельное количество строк для обработки устройством
   *
   * @param addr индекс устройства в каскаде, начиная с нуля
   * @param limit количество строк, 1..8
   */
  void setScanLimit(uint8_t addr, uint8_t limit);

  /**
   * @brief установить яркость свечения светодиодов устройства
   *
   * @param addr индекс устройства в каскаде, начиная с нуля
   * @param intensity яркость свечения, 0-15 (при значении 0 светодиоды не гаснут)
   */
  void setBrightness(uint8_t addr, uint8_t intensity);

  /**
   * @brief установить яркость свечения светодиодов для всех устройств
   *
   * @param intensity яркость свечения, 0-15 (при значении 0 светодиоды не гаснут)
   */
  void setBrightnessForAllDevices(uint8_t intensity);

  /**
   * @brief очистить устройство
   *
   * @param addr индекс устройства в каскаде, начиная с нуля
   * @param upd true - обновить изображение сразу, иначе очистится только буфер устройства, изображение будет обновлено только после вызова метода update
   */
  void clearDevice(uint8_t addr, bool upd = false);

  /**
   * @brief очистить все устройства
   *
   * @param upd true - обновить изображение сразу, иначе очистится только буфер устройств, изображение будет обновлено только после вызова метода update
   */
  void clearAllDevices(bool upd = false);

  /**
   * @brief установить состояние одиночного светодиода устройства с учетом нужного поворота и отражения изображения
   *
   * @param addr индекс устройства в каскаде, начиная с нуля
   * @param row строка (координата Y)
   * @param column столбец (координата X)
   * @param state устанавливаемое состояние светодиода
   * @param upd true - обновить изображение сразу, иначе изображение будет обновлено только после вызова метода update
   */
  void setLed(uint8_t addr, uint8_t row, uint8_t column, boolean state, bool upd = false);

  /**
   * @brief получение состояние одиночного светодиода устройства с учетом нужного поворота и отражения изображения
   *
   * @param addr индекс устройства в каскаде, начиная с нуля
   * @param row строка (координата Y)
   * @param column столбец (координата X)
   * @return true, если светодиод включен
   */
  bool getLedStat(uint8_t addr, uint8_t row, uint8_t column);

  /**
   * @brief установить строку устройства с учетом нужного поворота и отражения изображения
   *
   * @param addr индекс устройства в каскаде, начиная с нуля
   * @param row строка (координата Y)
   * @param value битовая маска, в которой каждый бит соотвествует состоянию соответствующего светодиода строки
   * @param upd true - обновить изображение сразу, иначе изображение будет обновлено только после вызова метода update
   */
  void setRow(uint8_t addr, uint8_t row, uint8_t value, bool upd = false);

  /**
   * @brief получение битовой маски строки из буфера устройства
   *
   * @param addr индекс устройства в каскаде, начиная с нуля
   * @param row строка (координата Y)
   * @return результат
   */
  uint8_t getRow(uint8_t addr, uint8_t row);

  /**
   * @brief установить столбец устройства с учетом нужного поворота и отражения изображения
   *
   * @param addr индекс устройства в каскаде, начиная с нуля
   * @param column столбец (координата X)
   * @param value битовая маска, в которой каждый бит соотвествует состоянию соответствующего светодиода столбца
   * @param upd true - обновить изображение сразу, иначе изображение будет обновлено только после вызова метода update
   */
  void setColumn(uint8_t addr, uint8_t column, uint8_t value, bool upd = false);

  /**
   * @brief получение битовой маски столбца из буфера устройства
   *
   * @param addr индекс устройства в каскаде, начиная с нуля
   * @param column столбец (координата X)
   * @return результат
   */
  uint8_t getColumn(uint8_t addr, uint8_t column);

  /**
   * @brief обновить все устройства
   *
   */
  void update();
};

// ---- private ---------------------------------

template <uint8_t csPin, uint8_t numDevices>
void shMAX72xxMini<csPin, numDevices>::_init()
{
  spiTransfer(OP_DISPLAYTEST, 0);
  spiTransfer(OP_SCANLIMIT, 7);
  spiTransfer(OP_DECODEMODE, 0);
  spiTransfer(OP_INTENSITY, 0);
  spiTransfer(OP_SHUTDOWN, 1);

  clearAllDevices(true);
}

#if MINICORE_AVR_ATMEGA328PB

template <uint8_t csPin, uint8_t numDevices>
void shMAX72xxMini<csPin, numDevices>::start(uint8_t spi)
{
  pinMode(csPin, OUTPUT);
  digitalWrite(csPin, HIGH);
  if (spi == PB_SPI1)
  {
    _spi1 = &SPI1;
    _spi1->begin();
  }
  else
  {
    _spi = &SPI;
    _spi->begin();
  }

  _init();
}

#else

template <uint8_t csPin, uint8_t numDevices>
void shMAX72xxMini<csPin, numDevices>::start()
{
  pinMode(csPin, OUTPUT);
  digitalWrite(csPin, HIGH);
  
  _spi->begin();

  _init();
}

#endif

#if defined(ARDUINO_ARCH_ESP32) || defined(ARDUINO_ARCH_ESP8266)
template <uint8_t csPin, uint8_t numDevices>
void shMAX72xxMini<csPin, numDevices>::start(int8_t sck_pin, int8_t mosi_pin, int8_t miso_pin)
{
  pinMode(csPin, OUTPUT);
  digitalWrite(csPin, HIGH);
#if defined(ARDUINO_ARCH_ESP32)
  _spi->begin(sck_pin, miso_pin, mosi_pin, csPin);
#else
  _spi->pins(sck_pin, miso_pin, mosi_pin, csPin);
  _spi->begin();
#endif

  _init();
}
#endif

template <uint8_t csPin, uint8_t numDevices>
void shMAX72xxMini<csPin, numDevices>::transfer_data()
{
#if MINICORE_AVR_ATMEGA328PB
  (is_spi1) ? _spi1->beginTransaction(SPI1Settings(spi_clock,
                                                   spi_bit_order,
                                                   spi_data_mode))
            : _spi->beginTransaction(SPISettings(spi_clock,
                                                 spi_bit_order,
                                                 spi_data_mode));
#else
  _spi->beginTransaction(spi_settings);
#endif

  digitalWrite(csPin, LOW);
  for (uint8_t i = numDevices * 2; i > 0; i--)
  {
#if MINICORE_AVR_ATMEGA328PB
    (is_spi1) ? _spi1->transfer(spidata[i - 1])
              : _spi->transfer(spidata[i - 1]);
#else
    _spi->transfer(spidata[i - 1]);
#endif
  }
  digitalWrite(csPin, HIGH);

#if MINICORE_AVR_ATMEGA328PB
  (is_spi1) ? _spi1->endTransaction()
            : _spi->endTransaction();
#else
  _spi->endTransaction();
#endif
}

template <uint8_t csPin, uint8_t numDevices>
void shMAX72xxMini<csPin, numDevices>::spiTransfer(uint8_t addr,
                                                   uint8_t opcode,
                                                   uint8_t data)
{
  uint8_t offset = addr * 2;

  for (uint8_t i = 0; i < numDevices * 2; i++)
  {
    spidata[i] = 0x00;
  }
  // готовим данные для отправки
  spidata[offset + 1] = opcode;
  spidata[offset] = data;

  // отправка данных
  transfer_data();
}

template <uint8_t csPin, uint8_t numDevices>
void shMAX72xxMini<csPin, numDevices>::spiTransfer(uint8_t opcode,
                                                   uint8_t data)
{
  // готовим данные для отправки
  for (uint8_t i = 0; i < numDevices * 2; i++)
  {
    // четный байт - данные, нечетный байт - опкод
    spidata[i] = ((i >> 0) & 0x01) ? opcode : data;
  }

  // отправка данных
  transfer_data();
}

template <uint8_t csPin, uint8_t numDevices>
uint8_t shMAX72xxMini<csPin, numDevices>::reverseByte(uint8_t b)
{
  b = (b & 0xF0) >> 4 | (b & 0x0F) << 4;
  b = (b & 0xCC) >> 2 | (b & 0x33) << 2;
  b = (b & 0xAA) >> 1 | (b & 0x55) << 1;
  return (b);
}

template <uint8_t csPin, uint8_t numDevices>
void shMAX72xxMini<csPin, numDevices>::_setColumn(uint8_t addr,
                                                  uint8_t col,
                                                  uint8_t value,
                                                  bool upd)
{
  uint8_t val;

  for (uint8_t row = 0; row < 8; row++)
  {
    val = value >> (7 - row);
    val = val & 0x01;
    _setLed(addr, row, col, val, false);
  }
  if (upd)
  {
    _update(addr);
  }
}

template <uint8_t csPin, uint8_t numDevices>
void shMAX72xxMini<csPin, numDevices>::_setRow(uint8_t addr,
                                               uint8_t row,
                                               uint8_t value,
                                               bool upd)
{
  uint8_t offset = addr * 8;
  status[offset + row] = value;
  if (upd)
  {
    spiTransfer(addr, row + 1, status[offset + row]);
  }
}

template <uint8_t csPin, uint8_t numDevices>
void shMAX72xxMini<csPin, numDevices>::_setLed(uint8_t addr,
                                               uint8_t row,
                                               uint8_t column,
                                               boolean state,
                                               bool upd)
{
  uint8_t offset = addr * 8;
  uint8_t val = 0x00;

  val = 0b10000000 >> column;
  if (state)
  {
    status[offset + row] = status[offset + row] | val;
  }
  else
  {
    val = ~val;
    status[offset + row] = status[offset + row] & val;
  }
  if (upd)
  {
    spiTransfer(addr, row + 1, status[offset + row]);
  }
}

template <uint8_t csPin, uint8_t numDevices>
bool shMAX72xxMini<csPin, numDevices>::_getLedState(uint8_t addr,
                                                    uint8_t row,
                                                    uint8_t column)
{
  uint8_t offset = addr * 8;
  bool result = (column < 8) ? (((status[offset + row]) >> (column)) & 0x01)
                             : false;
  return (result);
}

template <uint8_t csPin, uint8_t numDevices>
void shMAX72xxMini<csPin, numDevices>::_update(uint8_t addr,
                                               bool clear,
                                               bool transfer)
{
  uint8_t offset = addr * 8;
  for (uint8_t i = 0; i < 8; i++)
  {
    if (clear)
    {
      status[offset + i] = 0;
    }
    if (transfer)
    {
      spiTransfer(addr, i + 1, status[offset + i]);
    }
  }
}

// ---- public ----------------------------------

#if CUSTOM_SPI_PINS_AVAILABLE
template <uint8_t csPin, uint8_t numDevices>
void shMAX72xxMini<csPin, numDevices>::init(int8_t sck_pin, int8_t mosi_pin, int8_t miso_pin)
{
#if defined(ARDUINO_ARCH_RP2040)
  _spi->setSCK(sck_pin);
  _spi->setTX(mosi_pin);
#endif

#if defined(ARDUINO_ARCH_ESP32) || defined(ARDUINO_ARCH_ESP8266)
  start(sck_pin, mosi_pin, miso_pin);
#else
  start();
#endif
}
#endif

#if MINICORE_AVR_ATMEGA328PB

template <uint8_t csPin, uint8_t numDevices>
void shMAX72xxMini<csPin, numDevices>::init(uint8_t spi)
{
  is_spi1 = spi;
  start(spi);
}

#else

template <uint8_t csPin, uint8_t numDevices>
void shMAX72xxMini<csPin, numDevices>::init()
{
  start();
}

#endif

#if SPI1_AVAILABLE
template <uint8_t csPin, uint8_t numDevices>
void shMAX72xxMini<csPin, numDevices>::setSPI(shSPIClass *spi)
{
  if (spi != NULL)
  {
    _spi = spi;
  }
}
#endif

template <uint8_t csPin, uint8_t numDevices>
void shMAX72xxMini<csPin, numDevices>::setSPISettings(uint32_t clock,
                                                      uint8_t bitOrder,
                                                      uint8_t dataMode)
{
#if MINICORE_AVR_ATMEGA328PB
  spi_clock = clock;
  spi_bit_order = bitOrder;
  spi_data_mode = dataMode;
#else
  spi_settings = SPISettings(clock, bitOrder, dataMode);
#endif
}

template <uint8_t csPin, uint8_t numDevices>
void shMAX72xxMini<csPin, numDevices>::shutdownDevice(uint8_t addr,
                                                      bool off_device)
{
  if (addr >= numDevices)
  {
    return;
  }

  spiTransfer(addr, OP_SHUTDOWN, !off_device);
}

template <uint8_t csPin, uint8_t numDevices>
void shMAX72xxMini<csPin, numDevices>::shutdownAllDevices(bool off_device)
{
  spiTransfer(OP_SHUTDOWN, !off_device);
}

template <uint8_t csPin, uint8_t numDevices>
void shMAX72xxMini<csPin, numDevices>::setDirection(uint8_t dir)
{
  direct = dir % 4;
}

template <uint8_t csPin, uint8_t numDevices>
void shMAX72xxMini<csPin, numDevices>::setFlip(bool toFlip) { flip = toFlip; }

template <uint8_t csPin, uint8_t numDevices>
uint8_t shMAX72xxMini<csPin, numDevices>::getDeviceCount() { return (numDevices); }

template <uint8_t csPin, uint8_t numDevices>
void shMAX72xxMini<csPin, numDevices>::setScanLimit(uint8_t addr,
                                                    uint8_t limit)
{
  if (addr >= numDevices)
  {
    return;
  }

  limit = (limit == 0) ? 0 : (limit <= 8) ? limit - 1
                                          : 7;
  spiTransfer(addr, OP_SCANLIMIT, limit);
}

template <uint8_t csPin, uint8_t numDevices>
void shMAX72xxMini<csPin, numDevices>::setBrightness(uint8_t addr,
                                                     uint8_t intensity)
{
  if (addr >= numDevices)
  {
    return;
  }

  intensity = (intensity <= 15) ? intensity : 15;
  spiTransfer(addr, OP_INTENSITY, intensity);
}

template <uint8_t csPin, uint8_t numDevices>
void shMAX72xxMini<csPin, numDevices>::setBrightnessForAllDevices(uint8_t intensity)
{
  intensity = (intensity <= 15) ? intensity : 15;
  spiTransfer(OP_INTENSITY, intensity);
}

template <uint8_t csPin, uint8_t numDevices>
void shMAX72xxMini<csPin, numDevices>::clearDevice(uint8_t addr, bool upd)
{
  if (addr >= numDevices)
  {
    return;
  }

  _update(addr, true, upd);
}

template <uint8_t csPin, uint8_t numDevices>
void shMAX72xxMini<csPin, numDevices>::clearAllDevices(bool upd)
{
  for (uint8_t addr = 0; addr < numDevices; addr++)
  {
    _update(addr, true, upd);
  }
}

template <uint8_t csPin, uint8_t numDevices>
void shMAX72xxMini<csPin, numDevices>::setLed(uint8_t addr,
                                              uint8_t row,
                                              uint8_t column,
                                              boolean state,
                                              bool upd)
{
  if (addr >= numDevices || row > 7 || column > 7)
  {
    return;
  }

  uint8_t a = row;
  uint8_t b = column;
  switch (direct)
  {
  case 0:
    column = (flip) ? 7 - b : b;
    break;
  case 1:
    column = 7 - a;
    row = (flip) ? 7 - b : b;
    break;
  case 2:
    column = (flip) ? b : 7 - b;
    row = 7 - a;
    break;
  case 3:
    column = a;
    row = (flip) ? b : 7 - b;
    break;
  }
  _setLed(addr, row, column, state, upd);
}

template <uint8_t csPin, uint8_t numDevices>
bool shMAX72xxMini<csPin, numDevices>::getLedStat(uint8_t addr,
                                                  uint8_t row,
                                                  uint8_t column)
{
  bool result = false;
  if (addr >= numDevices || row > 7 || column > 7)
  {
    return (result);
  }

  uint8_t a = row;
  uint8_t b = column;

  switch (direct)
  {
  case 0:
    column = (flip) ? b : 7 - b;
    break;
  case 1:
    column = a;
    row = (flip) ? 7 - b : b;
    break;
  case 2:
    column = (flip) ? 7 - b : b;
    row = 7 - a;
    break;
  case 3:
    column = 7 - a;
    row = (flip) ? b : 7 - b;
    break;
  }

  return (_getLedState(addr, row, column));
}

template <uint8_t csPin, uint8_t numDevices>
void shMAX72xxMini<csPin, numDevices>::setRow(uint8_t addr,
                                              uint8_t row,
                                              uint8_t value,
                                              bool upd)
{
  if (addr >= numDevices || row > 7)
  {
    return;
  }

  switch (direct)
  {
  case 0:
    value = (flip) ? reverseByte(value) : value;
    break;
  case 1:
    value = (flip) ? reverseByte(value) : value;
    row = 7 - row;
    break;
  case 2:
    value = (flip) ? value : reverseByte(value);
    row = 7 - row;
    break;
  case 3:
    value = (flip) ? value : reverseByte(value);
    break;
  }
  (((direct) >> (0)) & 0x01) ? _setColumn(addr, row, value, upd)
                             : _setRow(addr, row, value, upd);
}

template <uint8_t csPin, uint8_t numDevices>
uint8_t shMAX72xxMini<csPin, numDevices>::getRow(uint8_t addr, uint8_t row)
{
  uint8_t result = 0x00;

  if (addr >= numDevices || row > 7)
  {
    return (result);
  }

  for (uint8_t i = 0; i < 8; i++)
  {
    uint8_t col = i;
    switch (direct)
    {
    case 0:
      (_getLedState(addr, row, (flip) ? 7 - col : col)) ? (result) |= (1UL << (i))
                                                        : (result) &= ~(1UL << (i));
      break;
    case 1:
      (_getLedState(addr, (flip) ? col : 7 - col, row)) ? (result) |= (1UL << (i))
                                                        : (result) &= ~(1UL << (i));
      break;
    case 2:
      (_getLedState(addr, 7 - row, (flip) ? 7 - col : col)) ? (result) |= (1UL << (i))
                                                            : (result) &= ~(1UL << (i));
      break;
    case 3:
      (_getLedState(addr, (flip) ? col : 7 - col, 7 - row)) ? (result) |= (1UL << (i))
                                                            : (result) &= ~(1UL << (i));
      break;
    }
  }
  if (direct > 1)
  {
    result = reverseByte(result);
  }

  return (result);
}

template <uint8_t csPin, uint8_t numDevices>
void shMAX72xxMini<csPin, numDevices>::setColumn(uint8_t addr,
                                                 uint8_t column,
                                                 uint8_t value,
                                                 bool upd)
{
  if (addr >= numDevices || column > 7)
  {
    return;
  }

  switch (direct)
  {
  case 0:
    column = (flip) ? 7 - column : column;
    break;
  case 1:
    column = (flip) ? 7 - column : column;
    value = reverseByte(value);
    break;
  case 2:
    column = (flip) ? column : 7 - column;
    value = reverseByte(value);
    break;
  case 3:
    column = (flip) ? column : 7 - column;
    break;
  }
  (((direct) >> (0)) & 0x01) ? _setRow(addr, column, value, upd)
                             : _setColumn(addr, column, value, upd);
}

template <uint8_t csPin, uint8_t numDevices>
uint8_t shMAX72xxMini<csPin, numDevices>::getColumn(uint8_t addr, uint8_t column)
{
  uint8_t result = 0x00;

  if (addr >= numDevices || column > 7)
  {
    return (result);
  }

  for (uint8_t i = 0; i < 8; i++)
  {
    uint8_t row = i;
    switch (direct)
    {
    case 0:
      column = (flip) ? column
                      : ((i == 0) ? 7 - column : column);
      break;
    case 1:
      column = (flip) ? ((i == 0) ? 7 - column : column)
                      : column;
      row = 7 - row;
      break;
    case 2:
      column = (flip) ? ((i == 0) ? 7 - column : column)
                      : column;
      break;
    case 3:
      column = (flip) ? column
                      : ((i == 0) ? 7 - column : column) /*?*/;
      break;
    }
    if (direct == 0 || direct == 2)
    {
      (_getLedState(addr, row, column)) ? (result) |= (1UL << (i))
                                        : (result) &= ~(1UL << (i));
    }
    else
    {
      (_getLedState(addr, column, row)) ? (result) |= (1UL << (i))
                                        : (result) &= ~(1UL << (i));
    }
  }

  if (direct == 0)
  {
    result = reverseByte(result);
  }

  return (result);
}

template <uint8_t csPin, uint8_t numDevices>
void shMAX72xxMini<csPin, numDevices>::update()
{
  for (uint8_t addr = 0; addr < numDevices; addr++)
  {
    _update(addr);
  }
}
// ==== end shMAX72xxMini ============================

// ==== shMAX72xx7Segment ============================

//      A
//     ---
//  F |   | B
//     -G-
//  E |   | C
//     --- *
//      D  X
const uint8_t PROGMEM digitToSegment[] = {
    // XABCDEFG
    0b01111110, // 0
    0b00110000, // 1
    0b01101101, // 2
    0b01111001, // 3
    0b00110011, // 4
    0b01011011, // 5
    0b01011111, // 6
    0b01110000, // 7
    0b01111111, // 8
    0b01111011, // 9
    0b01110111, // A
    0b00011111, // b
    0b01001110, // C
    0b00111101, // d
    0b01001111, // E
    0b01000111  // F
};

const uint8_t minusSegments = 0b00000001;

/**
 * @brief конструктор объекта
 *
 * @tparam csPin номер пина для подключения вывода CS
 * @tparam numDevices количество устройств в каскаде
 * @tparam numDigits количество знаков в каскаде (обычно по 8 на каждом устройстве)
 */
template <uint8_t csPin, uint8_t numDevices, uint16_t numDigits>
class shMAX72xx7Segment : public shMAX72xxMini<csPin, numDevices>
{
private:
public:
  shMAX72xx7Segment() : shMAX72xxMini<csPin, numDevices>() {}

  /**
   * @brief получение битовой маски числа для вывода на индикатор
   *
   * @param digit число (0..15)
   * @return uint8_t
   */
  uint8_t encodeDigit(uint8_t digit);

  /**
   * @brief получение количества знаков в каскаде
   *
   * @return uint8_t
   */
  uint16_t getNumDigits();
  /**
   * @brief вывод символа на индикатор
   *
   * @param index индекс индикатора в каскаде, начиная с нуля
   * @param value битовая маска выводимого символа
   * @param showDot показывать или нет десятичную точку
   * @param upd true - обновить изображение сразу, иначе изображение будет обновлено только после вызова метода update
   */
  void setChar(uint16_t index, uint8_t value, bool showDot = false, bool upd = false);
};

// ---- public ----------------------------------

template <uint8_t csPin, uint8_t numDevices, uint16_t numDigits>
uint8_t shMAX72xx7Segment<csPin, numDevices, numDigits>::encodeDigit(uint8_t digit)
{
  return ((digit < 16) ? pgm_read_byte(&digitToSegment[digit]) : 0x00);
}

template <uint8_t csPin, uint8_t numDevices, uint16_t numDigits>
uint16_t shMAX72xx7Segment<csPin, numDevices, numDigits>::getNumDigits() { return (numDigits); }

template <uint8_t csPin, uint8_t numDevices, uint16_t numDigits>
void shMAX72xx7Segment<csPin, numDevices, numDigits>::setChar(uint16_t index, uint8_t value, bool showDot, bool upd)
{
  if (index >= numDigits)
  {
    return;
  }

  uint8_t addr = index / 8;
  uint8_t idx = index % 8;
  if (showDot)
  {
    value |= 0b10000000;
  }
  shMAX72xxMini<csPin, numDevices>::setRow(addr, idx, value, upd);
}

// ==== end shMAX72xx7Segment ========================
