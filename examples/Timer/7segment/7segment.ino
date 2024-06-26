/**
 * @file Matrix.ino
 * @author Vladimir Shatalov (valesh-soft@yandex.ru)
 * 
 * @brief Пример создания таймера с выводом информации на семисегментный индикатор
 * 
 * @version 1.4
 * @date 15.04.2024
 *
 * @copyright Copyright (c) 2022
 */

#include <shMAX72xxMini.h>

#define CS_PIN 10
#define NUM_DEVICES 1
#define NUM_DIGITS 8

// инициируем модуль из одного устройства и четырех цифр, вывод CS модуля подулючен к пину D10 Ардуино, выводы DIN и CLK модуля подключены к пинам D11 и D13 Ардуино соответственно
shMAX72xx7Segment<CS_PIN, NUM_DEVICES, NUM_DIGITS> disp;

void setup()
{
  // инициализация драйвера
  disp.init();
  disp.setBrightness(0, 8);
}

void loop()
{
  static int secondCount = 120;
  if (secondCount >= 0)
  {
    // если время еще не закончилось, вывести его на экран
    disp.clearAllDevices();
    byte min = secondCount / 60;
    byte sec = secondCount % 60;
    // вывести минуты
    disp.setChar(7, disp.encodeDigit(min / 10));
    disp.setChar(6, disp.encodeDigit(min % 10), true);
    // вывести секунды
    disp.setChar(5, disp.encodeDigit(sec / 10));
    disp.setChar(4, disp.encodeDigit(sec % 10));

    disp.update();

    secondCount--;
  }
  else
  {
    // если время закончилось, трижды мигнуть экраном и запустить все заново
    for (byte i = 0; i < 6; i++)
    {
      disp.shutdownAllDevices(i % 2 == 0);
      delay(300);
    }
    secondCount = 120;
  }
  delay(1000);
}
