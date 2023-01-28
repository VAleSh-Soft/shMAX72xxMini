/**
 * @file Ticker.ino
 * @author Vladimir Shatalov (valesh-soft@yandex.ru)
 * @brief
 * @version 1.0
 * @date 20.11.2022
 *
 * @copyright Copyright (c) 2022
 *
 * Пример вывода бегущей строки на матрицу из четырех устройств
 */

#include "font.h"
#include <shMAX72xxMini.h>
#include <avr/pgmspace.h>

#define CS_PIN 10
#define NUM_DEVICES 4
#define LETTER_WIDTH 5      // ширина символа в пикселях
#define CHARACTER_SPACING 1 // отступ между символами в пикселях
#define TICKER_FPS 50       // частота смены кадров бегущей строки в секунду (fps)

// инициируем модуль из четырех устройств, вывод CS модуля подулючен к пину D10 Ардуино, выводы DIN и CLK модуля подключены к пинам D11 и D13 Ардуино соответственно
shMAX72xxMini<CS_PIN, NUM_DEVICES> disp;

// строка для вывода на экран
char ticker_string[] = "0123456789 - english string: Hellow! русская строка: Привет!";

uint8_t *data = NULL;    // буфер для вывода на экран
uint16_t data_count = 0; // размер буфера; равен количеству символов в строке, умноженному на 6 (LETTER_WIDTH + CHARACTER_SPASING) плюс количество столбцов на матрице (NUM_DEVICES * 8), чтобы новый проход строки начинался только после завершения предыдущего прохода

void setData()
{
  uint16_t i = 0;
  uint16_t n = 0;
  while (ticker_string[i] != NULL)
  {
    uint8_t chr = (int)ticker_string[i];
    // перекодировка кириллицы из UTF8 в Win-1521 при необходимости
    if (chr >= 0xC0)
    {
      switch (chr)
      {
      case 0xD0:
        i++;
        chr = (int)ticker_string[i];
        if (chr == 0x81)
        {
          chr = 0xA8;
        }
        else if (chr >= 0x90 && chr <= 0xBF)
        {
          chr += 0x30;
        }
        break;
      case 0xD1:
        i++;
        chr = (int)ticker_string[i];
        if (chr == 0x91)
        {
          chr = 0xB8;
        }
        else if (chr >= 0x80 && chr <= 0x8F)
        {
          chr += 0x70;
        }
        break;
      }
    }
    i++;

    // заполнение битовой маски символа в буфере
    for (uint8_t j = 0; j < LETTER_WIDTH; j++)
    {
      data[n + j] = pgm_read_byte(&font_5_7[chr * LETTER_WIDTH + j]);
    }
    n += LETTER_WIDTH + CHARACTER_SPACING;
  }
}

void setup()
{
  disp.setBrightnessForAllDevices(8);
  disp.setDirection(2); // установите нужный угол поворота
  // disp.setFlip(true);   // если нужно включить отражение изображения, раскомментируйте строку

  // определение размера буфера; 60 - количество символов в строке, включая пробелы и знаки препинания
  data_count = 60 * (LETTER_WIDTH + CHARACTER_SPACING) + NUM_DEVICES * 8;
  // выделение памяти под буфер
  data = (uint8_t *)calloc(data_count, sizeof(uint8_t));
  // если память выделена успешно, заполнение буфера битовыми масками символов
  if (data)
  {
    setData();
  }
}

void loop()
{
  static uint32_t timer = 0;
  static uint16_t n = 1;

  if (millis() - timer >= 1000 / TICKER_FPS)
  {
    timer = millis();

    // вывод очередного кадра
    disp.clearAllDevices();
    for (uint16_t i = NUM_DEVICES * 8, j = n; i > 0 && j > 0; i--, j--)
    {
      disp.setColumn((i - 1) / 8, (i - 1) % 8, reverseByte(data[j - 1]));
    }
    disp.update();

    // сдвиг позиции на один столбец для следующего кадра
    if (++n > data_count)
    {
      n = 1;
    }
  }
}
