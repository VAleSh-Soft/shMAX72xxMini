/**
 * @file FlipAndRotate.ino
 * @author Vladimir Shatalov (valesh-soft@yandex.ru)
 * @brief 
 * @version 1.0
 * @date 17.11.2022
 * 
 * @copyright Copyright (c) 2022
 * 
 * Пример, демонстрирующий поворот изображения и отражение его по горизонтали;
 * 
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

void setup()
{
  for (uint8_t d = 0; d < disp.getDeviceCount(); d++)
  {
    disp.setBrightness(d, 8);
  }
}

void loop()
{
  for (uint8_t i = 0; i < 4; i++)
  {
    // поочередно поворачиваем изображение во всех четырех направлениях
    disp.setDirection(i);
    for (uint8_t j = 0; j < 2; j++)
    {
      // поочередно включаем и выключаем зеркальное отражение
      disp.setFlip(j);
      for (uint8_t d = 0; d < disp.getDeviceCount(); d++)
      {
        // выводим изображение на все устройства
        for (uint8_t k = 0; k < sizeof(data); k++)
        {
          disp.setColumn(d, k, data[k]);
        }
      }
      // обновляем изображение
      disp.update();
      delay(2000);
      // очищаем все устройства
      disp.clearAllDevices(true);
    }
  }
}
