#include <Wire.h>
#include <Adafruit_LSM6DS3.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_GC9A01A.h>

// Конфигурация дисплея
#define TFT_CS   4
#define TFT_DC   6
#define TFT_RST  8
#define TFT_SCK  5
#define TFT_MOSI 7
Adafruit_GC9A01A tft(TFT_CS, TFT_DC, TFT_MOSI, TFT_SCK, TFT_RST);

// Акселерометр
Adafruit_LSM6DS3 lsm6ds3;
#define LSM6DS3_ADDRESS 0x6B

// Настройки
#define TEXT "HELLO!"
#define UPDATE_DELAY 500  // Задержка между обновлениями (мс)
#define STABLE_THRESHOLD 0.2f
#define ORIENTATION_THRESHOLD 0.7f

// Переменные для частичной очистки
int16_t prev_x, prev_y;
uint16_t prev_w, prev_h;
uint8_t current_rotation = 0;
bool first_draw = true;

void setup() {
  Serial.begin(115200);
  
  // Инициализация I2C
  Wire.setSDA(0);
  Wire.setSCL(1);
  Wire.begin();

  // Инициализация акселерометра
  if (!lsm6ds3.begin_I2C(LSM6DS3_ADDRESS)) {
    Serial.println("Ошибка LSM6DS3!");
    while(1);
  }
  lsm6ds3.setAccelRange(LSM6DS_ACCEL_RANGE_4_G);
  lsm6ds3.setAccelDataRate(LSM6DS_RATE_104_HZ);

  // Инициализация дисплея
  tft.begin();
  tft.setRotation(0);
  tft.fillScreen(GC9A01A_BLACK);
  tft.setTextColor(GC9A01A_WHITE);
  tft.setTextSize(3);
  
  // Первоначальный текст
  drawText();
}

void loop() {
  static uint32_t last_update = 0;
  if (millis() - last_update < UPDATE_DELAY) return;
  last_update = millis();

  sensors_event_t accel;
  lsm6ds3.getEvent(&accel, nullptr, nullptr);

  // Определение новой ориентации
  uint8_t new_rotation = current_rotation;
  if (fabs(accel.acceleration.x) > fabs(accel.acceleration.y)) {
    if (accel.acceleration.x > ORIENTATION_THRESHOLD) new_rotation = 3;
    else if (accel.acceleration.x < -ORIENTATION_THRESHOLD) new_rotation = 1;
  } else {
    if (accel.acceleration.y > ORIENTATION_THRESHOLD) new_rotation = 2;
    else if (accel.acceleration.y < -ORIENTATION_THRESHOLD) new_rotation = 0;
  }

  // Обновление только при изменении
  if (new_rotation != current_rotation || first_draw) {
    first_draw = false;
    clearPreviousText();
    current_rotation = new_rotation;
    tft.setRotation(current_rotation);
    drawText();
  }
}

void drawText() {
  int16_t x, y;
  uint16_t w, h;
  
  // Получаем размеры текста с текущим поворотом
  tft.getTextBounds(TEXT, 0, 0, &x, &y, &w, &h);
  
  // Корректируем позицию для центрирования
  int16_t cursor_x, cursor_y;
  if (current_rotation == 0 || current_rotation == 2) {
    cursor_x = (tft.width() - w) / 2;
    cursor_y = (tft.height() - h) / 2;
  } else {
    cursor_x = (tft.height() - h) / 2;
    cursor_y = (tft.width() - w) / 2;
  }
  
  // Сохраняем область текста для последующей очистки
  prev_x = cursor_x;
  prev_y = cursor_y;
  prev_w = w;
  prev_h = h;

  tft.setCursor(cursor_x, cursor_y);
  tft.print(TEXT);
}

void clearPreviousText() {
  // Очищаем только область предыдущего текста с учетом поворота
  tft.fillRect(prev_x, prev_y, prev_w, prev_h, GC9A01A_BLACK);
}
