#include <shMAX72xxMini.h>

#define CS_PIN 10

// инициируем модуль из четырех устройств, вывод CS модуля подулючен к пину D10 Ардуино
shMAX72xxMini<4> disp(CS_PIN);

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
// addr - индекс устройства в модуле
// offset - смещение цифры от края устройства
// index - выводимая цифра
void writeChar(uint8_t addr, uint8_t offset, uint8_t index)
{
  for (uint8_t i = 0; i < 6; i++)
  {
    disp.setColumn(addr, i + offset, font_digit[index * 6 + i]);
  }
}

void setup()
{ // включаем и настраиваем все устройства модуля
  for (uint8_t d = 0; d < disp.getDeviceCount(); d++)
  {
    disp.shutdownDevice(d, false);
    disp.setBrightness(d, 8);
  }
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
    writeChar(0, 1, min / 10);
    writeChar(1, 0, min % 10);
    // вывести секунды
    writeChar(2, 1, sec / 10);
    writeChar(3, 0, sec % 10);
    // отрисовать двоеточие
    disp.setColumn(1, 7, 0b00100100);
    disp.update();
  }
  else
  {
    // если время закончилось, трижды мигнуть экраном и запустить все заново
    for (uint8_t i = 0; i < 6; i++)
    {
      disp.shutdownAllDevices(i % 2 == 0);
      delay(300);
    }
    secondCount = 121;
  }
  secondCount--;
  delay(1000);
}