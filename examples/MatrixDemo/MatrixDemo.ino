#include <shMAX72xxMini.h>

#define CS_PIN 10

// инициируем модуль из четырех устройств, вывод CS модуля подулючен к пину D10 Ардуино
shMAX72xxMini<4> disp(CS_PIN);

uint32_t delay_time = 20;

// отображение символа на устройстве
void writeChar(byte addr, byte *data)
{
  for (byte i = 0; i < 5; i++)
  {
    disp.setColumn(addr, i, data[i]);
  }
}

// вывод слова
void writeWord()
{
  byte d[5] = {0b01111111, 0b01000001, 0b01000001, 0b01000001, 0b00111110};
  byte e[5] = {0b01111111, 0b01001001, 0b01001001, 0b01001001, 0b01000001};
  byte m[5] = {0b01111111, 0b00100000, 0b00011000, 0b00100000, 0b01111111};
  byte o[5] = {0b00111110, 0b01000001, 0b01000001, 0b01000001, 0b00111110};

  for (byte i = 0; i < 3; i++)
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
  byte val;
  for (byte i = 0; i < 4; i++)
  {
    val = (i % 2 == 0) ? 0xFF : 0x00;
    for (byte addr = 0; addr < disp.getDeviceCount(); addr++)
    {
      for (byte k = 0; k < 8; k++)
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
  for (byte i = 0; i < 2; i++)
  {
    for (byte row = 0; row <= 7; row++)
    {
      for (byte addr = 0; addr < disp.getDeviceCount(); addr++)
      {
        for (byte col = 0; col < 8; col++)
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
  // включаем и настраиваем все устройства модуля
  for (byte d = 0; d < disp.getDeviceCount(); d++)
  {
    disp.shutdownDevice(d, false);
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