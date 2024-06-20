/**
 * @file MatrixDemo.ino
 * @author Vladimir Shatalov (valesh-soft@yandex.ru)
 *
 * @brief  Пример, демонстрирующий вывод информации на матрицу из четырех
 *         устройств: анимированный вывод текста, заполнение матрицы по столбцам,
 *         построчно и попиксельно
 *
 * @version 1.5
 * @date 20.06.2024
 *
 * @copyright Copyright (c) 2022
 */

#include <shMAX72xxMini.h>

#define CS_PIN 10
#define NUM_DEVICES 4

// инициируем модуль из четырех устройств, вывод CS модуля подулючен к пину D10 Ардуино, выводы DIN и CLK модуля подключены к пинам D11 и D13 Ардуино соответственно
shMAX72xxMini<CS_PIN, NUM_DEVICES> matrix_display;

uint32_t delay_time = 20;

// отображение символа на устройстве
void writeChar(uint8_t addr, uint8_t *data, bool disapp = false)
{
  // символы выпадают сверху и уходят вниз
  for (int8_t i = 7; i >= 0; i--)
  {
    for (uint8_t j = 0; j < 5; j++)
    {
      uint8_t x = (disapp) ? 7 - i : i * -1;
      matrix_display.setColumn(addr, j, data[j], x, false);
    }
    matrix_display.update();
    delay(20);
  }
}

// вывод слова
void writeWord()
{
  uint8_t d[5] = {0b01111111, 0b01000001, 0b01000001, 0b01000001, 0b00111110};
  uint8_t e[5] = {0b01111111, 0b01001001, 0b01001001, 0b01001001, 0b01000001};
  uint8_t m[5] = {0b01111111, 0b00100000, 0b00011000, 0b00100000, 0b01111111};
  uint8_t o[5] = {0b00111110, 0b01000001, 0b01000001, 0b01000001, 0b00111110};

  for (uint8_t i = 0; i < 3; i++)
  {
    matrix_display.clearAllDevices();

    // появление текста
    writeChar(0, d);
    writeChar(1, e);
    writeChar(2, m);
    writeChar(3, o);

    // пару раз мигнуть текстом
    for (uint8_t k = 0; k < 2; k++)
    {
      delay(500);
      matrix_display.shutdownAllDevices(true);
      delay(500);
      matrix_display.shutdownAllDevices(false);
    }
    delay(500);

    // исчезновение текста
    writeChar(0, d, true);
    writeChar(1, e, true);
    writeChar(2, m, true);
    writeChar(3, o, true);
  }
}

// заполнение матрицы по строкам или по столбцам
void rowsAndColumns(bool rows)
{
  uint8_t val;
  for (uint8_t i = 0; i < 4; i++)
  {
    val = (i % 2 == 0) ? 0xFF : 0x00;
    for (uint8_t addr = 0; addr < matrix_display.getDeviceCount(); addr++)
    {
      for (uint8_t k = 0; k < 8; k++)
      {
        (rows) ? matrix_display.setRow(addr, k, val, true) : matrix_display.setColumn(addr, k, val, true);
        delay(delay_time);
      }
    }
  }
}

// заполнение матрицы бегущей точкой
void single()
{
  for (uint8_t i = 0; i < 2; i++)
  {
    for (uint8_t row = 0; row <= 7; row++)
    {
      for (uint8_t addr = 0; addr < matrix_display.getDeviceCount(); addr++)
      {
        for (uint8_t col = 0; col < 8; col++)
        {
          matrix_display.setLed(addr, row, col, !i, true);
          delay(delay_time);
        }
      }
    }
  }
}

void setup()
{
  // инициализация матрицы
  matrix_display.init();
  matrix_display.setBrightnessForAllDevices(8);
  matrix_display.setDirection(2); // установите нужный угол поворота
  // matrix_display.setFlip(true);   // если нужно включить отражение изображения, раскомментируйте строку
}

void loop()
{
  writeWord();
  rowsAndColumns(true);
  rowsAndColumns(false);
  single();
}
