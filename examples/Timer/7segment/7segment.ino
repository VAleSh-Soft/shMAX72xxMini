#include <shMAX72xxMini.h>

#define CS_PIN 10
#define NUM_DEVICES 1
#define NUM_DIGITS 4

// инициируем модуль из одного устройства и четырех цифр, вывод CS модуля подулючен к пину D10 Ардуино
shMAX72xx7Segment<CS_PIN, NUM_DEVICES, NUM_DIGITS> disp;

void setup()
{
  // включаем и настраиваем все устройства модуля
  disp.shutdownAllDevices(false);
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
    disp.setChar(0, disp.encodeDigit(min / 10));
    disp.setChar(1, disp.encodeDigit(min % 10), true);
    // вывести секунды
    disp.setChar(2, disp.encodeDigit(sec / 10));
    disp.setChar(3, disp.encodeDigit(sec % 10));

    disp.update();
  }
  else
  {
    // если время закончилось, трижды мигнуть экраном и запустить все заново
    for (byte i = 0; i < 6; i++)
    {
      disp.shutdownAllDevices(i % 2 == 0);
      delay(300);
    }
    secondCount = 121;
  }
  secondCount--;
  delay(1000);
}