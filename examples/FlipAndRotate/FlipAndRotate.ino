#include <shMAX72xxMini.h>

#define CS_PIN 10

// инициируем модуль из четырех устройств, вывод CS модуля подулючен к пину D10 Ардуино
shMAX72xxMini<4> disp(CS_PIN);

// битовая маска выводимого изображения (по столбцам)
uint8_t data[] = {
    0b00010010,
    0b00100100,
    0b11111000,
    0b10100100,
    0b00010010};

void setup()
{
  // включаем и настраиваем все устройства модуля
  for (uint8_t d = 0; d < disp.getDeviceCount(); d++)
  {
    disp.shutdownDevice(d, false);
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