/**
 * @file Matrix.ino
 * @author Vladimir Shatalov (valesh-soft@yandex.ru)
 * @brief
 * @version 1.0
 * @date 17.11.2022
 *
 * @copyright Copyright (c) 2022
 *
 * Пример создания таймера с выводом информации на матрицу из четырех устройств
 *
 */

#include <shMAX72xxMini.h>

#define CS_PIN 10
#define NUM_DEVICES 4

// инициируем модуль из четырех устройств, вывод CS модуля подулючен к пину D10 Ардуино, выводы DIN и CLK модуля подключены к пинам D11 и D13 Ардуино соответственно
shMAX72xxMini<CS_PIN, NUM_DEVICES> disp;

// цифры 6x8
const uint8_t font_digit[] = {
    0x7E, 0x85, 0x89, 0x91, 0xA1, 0x7E, // 0
    0x00, 0x00, 0x41, 0xFF, 0x01, 0x00, // 1
    0x43, 0x85, 0x89, 0x89, 0x89, 0x73, // 2
    0x42, 0x81, 0x91, 0x91, 0x91, 0x6E, // 3
    0x0C, 0x14, 0x24, 0x44, 0x84, 0xFF, // 4
    0xF2, 0xA1, 0xA1, 0xA1, 0xA1, 0x9E, // 5
    0x7E, 0x91, 0x91, 0x91, 0x91, 0x4E, // 6
    0xC0, 0x80, 0x87, 0x88, 0x90, 0xE0, // 7
    0x6E, 0x91, 0x91, 0x91, 0x91, 0x6E, // 8
    0x72, 0x89, 0x89, 0x89, 0x89, 0x7E, // 9
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00  // space
};

// отображение символа на устройстве
// offset - смещение цифры от края матрицы
// index - выводимая цифра
void writeChar(uint8_t offset, uint8_t index)
{
  for (uint8_t i = 0; i < 6; i++)
  {
    disp.setColumn(offset / 8, offset % 8 + i, font_digit[index * 6 + i]);
  }
}

void setup()
{
  disp.setBrightnessForAllDevices(8);

  disp.setDirection(2); // установите нужный угол поворота
  // disp.setFlip(true); // если нужно включить отражение изображения, раскомментируйте строку
}

void loop()
{
  static int secondCount = 120;
  if (secondCount >= 0)
  {
    // если время еще не закончилось, вывести его на экран
    disp.clearAllDevices();
    uint8_t min = secondCount / 60;
    uint8_t sec = secondCount % 60;
    // вывести минуты
    writeChar(1, min / 10);
    writeChar(8, min % 10);
    // вывести секунды
    writeChar(17, sec / 10);
    writeChar(24, sec % 10);
    // отрисовать двоеточие
    disp.setColumn(1, 7, 0b00100100);

    disp.update();

    secondCount--;
  }
  else
  {
    // если время закончилось, трижды мигнуть экраном и запустить все заново
    for (uint8_t i = 0; i < 6; i++)
    {
      disp.shutdownAllDevices(i % 2 == 0);
      delay(300);
    }
    secondCount = 120;
  }
  delay(1000);
}
