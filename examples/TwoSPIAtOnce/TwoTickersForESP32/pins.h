#pragma once

// ==== пины для подключения первой и второй матриц ==

// ---- esp32 ------------------------------
#if CONFIG_IDF_TARGET_ESP32 

#define CS1_PIN 5
#define CLK1_PIN 18
#define DIN1_PIN 23
#define MISO1_PIN 19

#define CS2_PIN 14
#define CLK2_PIN 15
#define DIN2_PIN 13
#define MISO2_PIN 12

// ---- esp32s3 / esp32s2 ------------------
#elif CONFIG_IDF_TARGET_ESP32S3 || CONFIG_IDF_TARGET_ESP32S2

#define CS1_PIN 9
#define CLK1_PIN 12
#define DIN1_PIN 11
#define MISO1_PIN 13

#define CS2_PIN 8
#define CLK2_PIN 7
#define DIN2_PIN 6
#define MISO2_PIN 5

// ---- esp32c3 ---------------------------------
#elif CONFIG_IDF_TARGET_ESP32C3

#define CS1_PIN 7
#define CLK1_PIN 4
#define DIN1_PIN 6
#define MISO1_PIN 5

#define CS2_PIN 8
#define CLK2_PIN 3
#define DIN2_PIN 2
#define MISO2_PIN 1

#endif
