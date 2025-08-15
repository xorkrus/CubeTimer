#include <Wire.h>
#include <Adafruit_LSM6DS3.h>
#include <TB_TFT_eSPI.h>
#include "T3X5_54pt7b.h"

Adafruit_LSM6DS3 lsm6ds3;
TFT_eSPI tft = TFT_eSPI();

void setup() {
  tft.setFreeFont(&T3X5_54pt7b);
  // Инициализация I2C
  Wire.setSDA(0);
  Wire.setSCL(1);
  Wire.begin();

  // Инициализация датчика
  if (!lsm6ds3.begin_I2C(0x6B)) {
    while (1);
  }

  // Настройка дисплея
  tft.init();
  tft.setRotation(0);
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_WHITE);
  // tft.setTextFont(2);
  tft.setTextDatum(MC_DATUM);
}

void loop() {
  sensors_event_t accel;
  lsm6ds3.getEvent(&accel, NULL, NULL);

  // Определение положения
  String position = "UNKNOWN";
  uint8_t rotation = 0;
  
  // Проверка всех положений с допуском ±5
  if (inRange(accel, 0, 0, 10)) {
    tft.fillScreen(TFT_BLACK);
    position = "BASE";
    rotation = 0;
  } 
  else if (inRange(accel, 0, 10, 0)) {
    tft.fillScreen(TFT_RED);
    position = "RIGHT";
    rotation = 3;
  }
  else if (inRange(accel, 0, -10, 0)) {
    tft.fillScreen(TFT_GREEN);
    position = "LEFT";
    rotation = 1;
  }
  else if (inRange(accel, 0, 0, -10)) {
    tft.fillScreen(TFT_YELLOW);
    position = "INVERTED";
    rotation = 2;
  }
  else if (inRange(accel, -10, 0, 0)) {
    tft.fillScreen(TFT_BLUE);
    position = "BACK";
    rotation = 0;
  }
  else if (inRange(accel, 10, 0, 0)) {
    tft.fillScreen(TFT_CYAN);
    position = "FRONT";
    rotation = 0;
  }

  // Применяем поворот
  tft.setRotation(rotation);
  tft.drawString(position, tft.width()/2, tft.height()/2);
  delay(1000);
}

bool inRange(sensors_event_t accel, float x, float y, float z) {
  const float threshold = 5.0;
  return (abs(accel.acceleration.x - x) < threshold) &&
         (abs(accel.acceleration.y - y) < threshold) &&
         (abs(accel.acceleration.z - z) < threshold);
}
