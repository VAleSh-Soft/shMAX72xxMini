#pragma once

#include <avr/pgmspace.h>
#include <Arduino.h>
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

// numDevices - количество устройств в каскаде
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

  // отправка одиночной команды в устройство
  void spiTransfer(uint8_t addr, uint8_t opcode, uint8_t data)
  {
    uint8_t offset = addr * 2;
    uint8_t maxbytes = numDevices * 2;

    for (uint8_t i = 0; i < maxbytes; i++)
    {
      spidata[i] = 0x00;
    }
    // готовим данные для отправки
    spidata[offset + 1] = opcode;
    spidata[offset] = data;

    // отправка данных
    digitalWrite(csPin, LOW);
    for (uint8_t i = maxbytes; i > 0; i--)
    {
      SPI.transfer(spidata[i - 1]);
    }
    digitalWrite(csPin, HIGH);
  }

  // изменение порядка следования битов в байте
  uint8_t reverseByte(uint8_t b)
  {
    b = (b & 0xF0) >> 4 | (b & 0x0F) << 4;
    b = (b & 0xCC) >> 2 | (b & 0x33) << 2;
    b = (b & 0xAA) >> 1 | (b & 0x55) << 1;
    return (b);
  }

  // отрисовка столбца
  void _setColumn(uint8_t addr, uint8_t col, uint8_t value, bool upd)
  {
    uint8_t val;

    for (uint8_t row = 0; row < 8; row++)
    {
      val = value >> (7 - row);
      val = val & 0x01;
      _setLed(addr, row, col, val, upd);
    }
  }

  // отрисовка строки
  void _setRow(uint8_t addr, uint8_t row, uint8_t value, bool upd)
  {
    uint8_t offset = addr * 8;
    status[offset + row] = value;
    if (upd)
    {
      spiTransfer(addr, row + 1, status[offset + row]);
    }
  }

  // отрисовка одиночного светодиода
  void _setLed(uint8_t addr, uint8_t row, uint8_t column, boolean state, bool upd)
  {
    uint8_t offset = addr * 8;
    uint8_t val = 0x00;

    val = B10000000 >> column;
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

  // обновление; возможно обновление как только буфера, так и изображения на матрице; возможно одновременное стирание буфера (и матрицы, соответсвенно)
  void _update(uint8_t addr, bool clear = false, bool transfer = true)
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

public:
  // Подключение устройства к Ардуино:
  // DataIn - к пину D11
  // CLK    - к пину D13
  // CS     - к пину csPin (обычно это пин D10)
  // numDevices - количество устройств в каскаде
  shMAX72xxMini()
  {
    pinMode(csPin, OUTPUT);
    digitalWrite(csPin, HIGH);
    SPI.begin();
    for (uint8_t i = 0; i < numDevices; i++)
    {
      spiTransfer(i, OP_DISPLAYTEST, 0);
      spiTransfer(i, OP_SCANLIMIT, 7);
      spiTransfer(i, OP_DECODEMODE, 0);
      spiTransfer(i, OP_SHUTDOWN, 0);
      clearDevice(i, true);
    }
  }

  // отключить или включить устройство
  // addr - индекс устройства в каскаде, начиная с нуля
  // off_device - true - отключить устройство, false - включить устройство
  void shutdownDevice(uint8_t addr, bool off_device)
  {
    if (addr >= numDevices)
    {
      return;
    }

    spiTransfer(addr, OP_SHUTDOWN, !off_device);
  }

  // отключить или включить все устройства в каскаде
  // off_device - true - отключить устройства, false - включить устройства
  void shutdownAllDevices(bool off_device)
  {
    for (uint8_t addr = 0; addr < numDevices; addr++)
    {
      spiTransfer(addr, OP_SHUTDOWN, !off_device);
    }
  }

  // установить угол поворота изображения на всех устройствах
  // dir - угол поворота изображения, 0-3
  void setDirection(uint8_t dir) { direct = dir % 4; }

  // включить отражение изображения по горизонтали (по строкам) на всех устройствах
  // toFlip - true - включить отражение false - отключить отражение
  void setFlip(bool toFlip) { flip = toFlip; }

  // получить количество устройств в каскаде
  uint8_t getDeviceCount() { return (numDevices); }

  // установить количество строк для обработки устройством
  // addr - индекс устройства в каскаде, начиная с нуля
  // limit - количество строк, 1-8
  void setScanLimit(uint8_t addr, uint8_t limit)
  {
    if (addr >= numDevices)
    {
      return;
    }

    limit = (limit == 0) ? 0 : (limit <= 8) ? limit - 1 : 7;
    spiTransfer(addr, OP_SCANLIMIT, limit);
  }

  // установить яркость свечения светодиодов устройства
  // addr - индекс устройства в каскаде, начиная с нуля
  // intensity - яркость свечения, 0-15 (при значении 0 светодиоды не гаснут)
  void setBrightness(uint8_t addr, uint8_t intensity)
  {
    if (addr >= numDevices)
    {
      return;
    }

    intensity = (intensity <= 15) ? intensity : 15;
    spiTransfer(addr, OP_INTENSITY, intensity);
  }

  // очистить устройство
  // addr - индекс устройства в каскаде, начиная с нуля
  // upd - true - обновить изображение сразу, иначе очистится только буфер устройства, изображение будет обновлено только после вызова метода update
  void clearDevice(uint8_t addr, bool upd = false)
  {
    if (addr >= numDevices)
    {
      return;
    }

    _update(addr, true, upd);
  }

  // очистить все устройства
  // upd - true - обновить изображение сразу, иначе очистится только буфер устройств, изображение будет обновлено только после вызова метода update
  void clearAllDevices(bool upd = false)
  {
    for (uint8_t addr = 0; addr < numDevices; addr++)
    {
      _update(addr, true, upd);
    }
  }

  // установить состояние одиночного светодиода устройства с учетом нужного поворота и отражения изображения
  // addr - индекс устройства в каскаде, начиная с нуля
  // row - строка (координата Y)
  // column - столбец (координата X)
  // state - устанавливаемое состояние светодиода
  // upd - true - обновить изображение сразу, иначе изображение будет обновлено только после вызова метода update
  void setLed(uint8_t addr, uint8_t row, uint8_t column, boolean state, bool upd = false)
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

  // установить строку устройства с учетом нужного поворота и отражения изображения
  // addr - индекс устройства в каскаде, начиная с нуля
  // row - строка (координата Y)
  // value - битовая маска, в которой каждый бит соотвествует состоянию соответствующего светодиода строки
  // upd - true - обновить изображение сразу, иначе изображение будет обновлено только после вызова метода update
  void setRow(uint8_t addr, uint8_t row, uint8_t value, bool upd = false)
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

  // установить столбец устройства с учетом нужного поворота и отражения изображения
  // addr - индекс устройства в каскаде, начиная с нуля
  // column - столбец (координата X)
  // value - битовая маска, в которой каждый бит соотвествует состоянию соответствующего светодиода столбца
  // upd - true - обновить изображение сразу, иначе изображение будет обновлено только после вызова метода update
  void setColumn(uint8_t addr, uint8_t column, uint8_t value, bool upd = false)
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

  // обновить все устройства
  void update()
  {
    for (uint8_t addr = 0; addr < numDevices; addr++)
    {
      _update(addr);
    }
  }
};

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

template <uint8_t csPin, uint8_t numDevices, uint8_t numDigits>
class shMAX72xx7Segment : public shMAX72xxMini<csPin, numDevices>
{
private:
public:
  shMAX72xx7Segment() : shMAX72xxMini<csPin, numDevices>() {}

  uint8_t encodeDigit(uint8_t digit)
  {
    return ((digit < 16) ? pgm_read_byte(&digitToSegment[digit]) : 0x00);
  }

  uint8_t getNumDigits() { return (numDigits); }

  void setChar(uint8_t index, uint8_t value, bool showDot = false, bool upd = false)
  {
    if (index >= numDigits)
    {
      return;
    }

    uint8_t addr = index / 8;
    index -= addr * 8;
    if (showDot)
    {
      value |= 0b10000000;
    }
    shMAX72xxMini<csPin, numDevices>::setRow(addr, index, value, upd);
  }
};
