/**
 * @file CopyData.ino
 * @author Vladimir Shatalov (valesh-soft@yandex.ru)
 * 
 * @brief Пример считывания данных из буфера устройства и вывод его на другое 
 *        устройство; в этом примере данные выводятся на первое и второе 
 *        устройства каскада, после чего копируются на третье и четвертое 
 *        стройства; при копировании учитываются  заданные для матрицы поворот 
 *        и отражение изображения; 
 * 
 * @version 1.4
 * @date 15.04.2024
 * 
 * @copyright Copyright (c) 2022
 */

#include <shMAX72xxMini.h>

#define CS_PIN 10
#define NUM_DEVICES 4

// инициируем модуль из четырех устройств, вывод CS модуля подулючен к пину D10 Ардуино, выводы DIN и CLK модуля подключены к пинам D11 и D13 Ардуино соответственно
shMAX72xxMini<CS_PIN, NUM_DEVICES> disp;

// битовая маска выводимого изображения (по столбцам)
uint8_t data[] = {
    0b00010010,
    0b00100100,
    0b11111000,
    0b10100100,
    0b00010010};

// отрисовка изображения-источника
void setData(uint8_t _type)
{
  disp.clearAllDevices();
  // выводим изображение на первое устройства
  for (uint8_t i = 0; i < sizeof(data); i++)
  {
    disp.setColumn(0, i, data[i]);
  }
  // выводим линию на второе устройство
  switch (_type)
  {
  case 0: // столбец
    disp.setColumn(1, 4, 0xff);
    break;
  case 1: // строка
    disp.setRow(1, 4, 0xff);
    break;
  case 2: // косая черта
    for (uint8_t i = 0; i < 8; i++)
    {
      disp.setLed(1, i, i, true);
    }
    break;
  }
}

// копирование изображения
void copyData(uint8_t _type)
{
  for (uint8_t i = 0; i < 2; i++)
  {
    for (uint8_t j = 0; j < 8; j++)
    {
      switch (_type)
      {
      case 0: // перенос по столбцам
        disp.setColumn(i + 2, j, disp.getColumn(i, j));
        break;
      case 1: // перенос по строкам
        disp.setRow(i + 2, j, disp.getRow(i, j));
        break;
      case 2: // перенос попиксельно
        for (uint8_t k = 0; k < 8; k++)
        {
          disp.setLed(i + 2, j, k, disp.getLedStat(i, j, k));
        }
        break;
      }
    }
  }
}

void runCopy(uint8_t _type)
{
  for (uint8_t i = 0; i < 4; i++)
  {
    // поочередно поворачиваем изображение во всех четырех направлениях
    disp.setDirection(i);
    for (uint8_t j = 0; j < 2; j++)
    {
      // поочередно включаем и выключаем зеркальное отражение
      disp.setFlip(j);
      setData(_type);
      copyData(_type);
      disp.update();
      delay(1000);
    }
  }
}

void setup()
{
  // инициализация матрицы
  disp.init();
  disp.setBrightnessForAllDevices(8);
}

void loop()
{
  runCopy(0);
  runCopy(1);
  runCopy(2);
}
