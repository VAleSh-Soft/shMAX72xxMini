/**
 * @file MatrixDemo.ino
 * @author Vladimir Shatalov (valesh-soft@yandex.ru)
 * @brief 
 * @version 1.0
 * @date 17.11.2022
 * 
 * @copyright Copyright (c) 2022
 * 
 * Пример, демонстрирующий вывод информации на матрицу из четырех устройств: вывод текста,
 * заполнение матрицы по столбцам, построчно и попиксельно
 * 
 */

#include <shMAX72xxMini.h>

#define CS_PIN 10
#define NUM_DEVICES 4

// инициируем модуль из четырех устройств, вывод CS модуля подулючен к пину D10 Ардуино, выводы DIN и CLK модуля подключены к пинам D11 и D13 Ардуино соответственно
shMAX72xxMini<CS_PIN, NUM_DEVICES> disp;

uint32_t delay_time = 20;

// отображение символа на устройстве
void writeChar(uint8_t addr, uint8_t *data)
{
  for (uint8_t i = 0; i < 5; i++)
  {
    disp.setColumn(addr, i, data[i]);
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
    writeChar(0, d);
    writeChar(1, e);
    writeChar(2, m);
    writeChar(3, o);
    disp.update();
    delay(1000);
    disp.clearAllDevices(true);
    delay(500);
  }
}

// заполнение матрицы по строкам или по столбцам
void rowsAndColumns(bool rows)
{
  uint8_t val;
  for (uint8_t i = 0; i < 4; i++)
  {
    val = (i % 2 == 0) ? 0xFF : 0x00;
    for (uint8_t addr = 0; addr < disp.getDeviceCount(); addr++)
    {
      for (uint8_t k = 0; k < 8; k++)
      {
        (rows) ? disp.setRow(addr, k, val, true) : disp.setColumn(addr, k, val, true);
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
      for (uint8_t addr = 0; addr < disp.getDeviceCount(); addr++)
      {
        for (uint8_t col = 0; col < 8; col++)
        {
          disp.setLed(addr, row, col, !i, true);
          delay(delay_time);
        }
      }
    }
  }
}

void setup()
{
  for (uint8_t d = 0; d < disp.getDeviceCount(); d++)
  {
    disp.setBrightness(d, 8);
  }
  disp.setDirection(2); // установите нужный угол поворота
  // disp.setFlip(true);   // если нужно включить отражение изображения, раскомментируйте строку
}

void loop()
{
  writeWord();
  rowsAndColumns(true);
  rowsAndColumns(false);
  single();
}
