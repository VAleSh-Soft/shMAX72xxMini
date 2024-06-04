/**
 * @file TwoTickerForAtmega328pb.ino
 * @author Vladimir Shatalov (valesh-soft@yandex.ru)
 *
 * @brief Пример одновременного использования двух SPI-интерфейсов для
 *        независимого вывода данных на каждый;
 *
 *        Скетч написан для использования на ATmega328pb с использованием
 *        аддона MiniCore
 *
 *        В примере используется одновременный вывод бегущих строк на
 *        две матрицы из четырех модулей каждая;
 *
 * @version 1.0
 * @date 04.06.2024
 *
 * @copyright Copyright (c) 2024
 *
 */

#include "font.h"
#include <shMAX72xxMini.h>
#include <avr/pgmspace.h>

// пины для подключения первой бегущей строки
#define CS_PIN 3
#define CLK_PIN 13
#define DIN_PIN 11

// пины для подключения второй бегущей строки
#define CS1_PIN 4
#define CLK1_PIN A1
#define DIN1_PIN A7

#define NUM_DEVICES 4

#define LETTER_WIDTH 5      // ширина символа в пикселях
#define CHARACTER_SPACING 1 // отступ между символами в пикселях
#define TICKER_FPS 50       // частота смены кадров бегущей строки в секунду (fps)

// инициируем два модуля из четырех устройств
shMAX72xxMini<CS_PIN, NUM_DEVICES> first_display;
shMAX72xxMini<CS1_PIN, NUM_DEVICES> second_display;

// строки для вывода на экран
char first_string[] = "SPI - first SPI interface; первый SPI-интерфейс";
char second_string[] = "SPI1 - second SPI interface; второй SPI-интерфейс";

uint8_t *first_data = NULL;  // буфер для вывода на первый экран
uint8_t *second_data = NULL; // буфер для вывода на второй экран

uint16_t first_data_count = 0;  // размер первого буфера;
uint16_t second_data_count = 0; // размер второго буфера;

/*
 * размер буфера равен количеству символов в строке, умноженному на 6
 * (LETTER_WIDTH + CHARACTER_SPASING) плюс количество столбцов на матрице
 * (NUM_DEVICES * 8), чтобы новый проход строки начинался только после
 * завершения предыдущего прохода
 */

// определение количества символов в строке; подсчет символов делается с учетом
// кириллицы (по два байта на символ)
uint16_t getLengthOfString(char *_str)
{
  uint16_t result = 0;
  for (uint16_t i = 0; i < UINT16_MAX; i++)
  {
    // считаем до первого нулевого символа, не учитывая служебные байты
    if ((uint8_t)_str[i] > 0)
    {
      // символы латиницы считаем без сомнения ))
      if ((uint8_t)_str[i] < 0xc0)
      {
        result++;
      }
      // иначе, если первый байт служебный - пропускаем его
      else if ((uint8_t)_str[i] != 0xd0 && (uint8_t)_str[i] != 0xd1)
      {
        result++;
      }
    }
    else
    {
      break;
    }
  }

  return (result);
}

// заполнение буфера экрана данными
void setData(char *_str, uint8_t *_data)
{
  uint16_t i = 0;
  uint16_t n = 0;
  while (_str[i] != 0)
  {
    uint8_t chr = (uint8_t)_str[i];
    // перекодировка кириллицы из UTF8 в Win-1521 при необходимости
    if (chr >= 0xC0)
    {
      switch (chr)
      {
      case 0xD0:
        i++;
        chr = (uint8_t)_str[i];
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
        chr = (uint8_t)_str[i];
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
      _data[n + j] = pgm_read_byte(&font_5_7[chr * LETTER_WIDTH + j]);
    }
    n += LETTER_WIDTH + CHARACTER_SPACING;
  }
}

void setup()
{
  // инициализация первого SPI-интерфейса
  first_display.init();
  first_display.setBrightnessForAllDevices(4);
  first_display.setDirection(2); // установите нужный угол поворота

  first_data_count = getLengthOfString(first_string) * (LETTER_WIDTH + CHARACTER_SPACING) + NUM_DEVICES * 8;
  // выделение памяти под буфер
  // first_data = new uint8_t[first_data_count];
  first_data = (uint8_t *)calloc(first_data_count, sizeof(uint8_t));
  // если память выделена успешно, заполнение буфера битовыми масками символов
  if (first_data != NULL)
  {
    setData(first_string, first_data);
  }
  
  // инициализация второго SPI-интерфейса
  second_display.init(PB_SPI1);
  second_display.setBrightnessForAllDevices(4);
  second_display.setDirection(2); // установите нужный угол поворота

  second_data_count = getLengthOfString(second_string) * (LETTER_WIDTH + CHARACTER_SPACING) + NUM_DEVICES * 8;
  // second_data = new uint8_t[second_data_count];
  second_data = (uint8_t *)calloc(second_data_count, sizeof(uint8_t));
  // если память выделена успешно, заполнение буфера битовыми масками символов
  if (second_data != NULL)
  {
    setData(second_string, second_data);
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
    first_display.clearAllDevices();
    for (uint16_t i = NUM_DEVICES * 8, j = n; i > 0 && j > 0; i--, j--)
    {
      first_display.setColumn((i - 1) / 8, (i - 1) % 8, reverseByte(first_data[j - 1]));
    }
    first_display.update();

    // сдвиг позиции на один столбец для следующего кадра
    if (++n > first_data_count)
    {
      n = 1;
    }
  }
  
  static uint32_t timer1 = 0;
  static uint16_t m = 1;

  if (millis() - timer1 >= 1000 / TICKER_FPS)
  {
    timer1 = millis();

    // вывод очередного кадра
    second_display.clearAllDevices();
    for (uint16_t i = NUM_DEVICES * 8, j = m; i > 0 && j > 0; i--, j--)
    {
      second_display.setColumn((i - 1) / 8, (i - 1) % 8, reverseByte(second_data[j - 1]));
    }
    second_display.update();

    // сдвиг позиции на один столбец для следующего кадра
    if (++m > second_data_count)
    {
      m = 1;
    }
  }
}
