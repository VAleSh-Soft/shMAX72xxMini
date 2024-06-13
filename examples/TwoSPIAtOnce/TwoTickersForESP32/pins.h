#pragma once

// ==== пины для подключения первой и второй матриц ==

// ---- esp32 ------------------------------
#if CONFIG_IDF_TARGET_ESP32 

#define CS_PIN 5
#define CLK_PIN 18
#define DIN_PIN 23
#define MISO_PIN 19

#define CS1_PIN 14
#define CLK1_PIN 15
#define DIN1_PIN 13
#define MISO1_PIN 12

// ---- esp32s3 / esp32s2 ------------------
#elif CONFIG_IDF_TARGET_ESP32S3 || CONFIG_IDF_TARGET_ESP32S2

#define CS_PIN 9
#define CLK_PIN 12
#define DIN_PIN 11
#define MISO_PIN 13

#define CS1_PIN 8
#define CLK1_PIN 7
#define DIN1_PIN 6
#define MISO1_PIN 5

#endif
