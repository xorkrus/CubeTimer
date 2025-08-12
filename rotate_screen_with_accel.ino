#include <Wire.h>
#include <Adafruit_LSM6DS3.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_GC9A01A.h>

// Конфигурация пинов дисплея
#define TFT_CS   4
#define TFT_DC   6
#define TFT_RST  8
#define TFT_SCK  5
#define TFT_MOSI 7

// Инициализация дисплея
Adafruit_GC9A01A tft(TFT_CS, TFT_DC, TFT_MOSI, TFT_SCK, TFT_RST);

// Инициализация акселерометра
Adafruit_LSM6DS3 lsm6ds3;
#define LSM6DS3_ADDRESS 0x6B

// Параметры отображения
#define CENTER_X 120
#define CENTER_Y 120
#define TEXT "HELLO!"
#define STABLE_THRESHOLD 0.2f  // Порог стабильности (в g)
#define ORIENTATION_THRESHOLD 0.7f  // Порог определения ориентации (в g)

// Фильтр для стабилизации показаний
float filteredX = 0, filteredY = 0, filteredZ = 0;
const float filterAlpha = 0.1;  // Коэффициент фильтрации

void setup() {
  Serial.begin(115200);
  
  // Инициализация I2C
  Wire.setSDA(0);
  Wire.setSCL(1);
  Wire.begin();

  // Инициализация акселерометра
  if (!lsm6ds3.begin_I2C(LSM6DS3_ADDRESS)) {
    Serial.println("Ошибка инициализации LSM6DS3!");
    while (1);
  }
  
  lsm6ds3.setAccelRange(LSM6DS_ACCEL_RANGE_4_G);
  lsm6ds3.setAccelDataRate(LSM6DS_RATE_104_HZ);

  // Инициализация дисплея
  tft.begin();
  tft.setRotation(0);
  tft.fillScreen(GC9A01A_BLACK);
  tft.setTextColor(GC9A01A_WHITE);
  tft.setTextSize(3);
  tft.setTextWrap(false);
  
  // Первоначальное отображение текста
  drawCenteredText();
}

void loop() {
  // Получаем данные акселерометра
  sensors_event_t accel;
  lsm6ds3.getEvent(&accel, nullptr, nullptr);

  // Применяем простой фильтр низких частот
  filteredX = filterAlpha * accel.acceleration.x + (1 - filterAlpha) * filteredX;
  filteredY = filterAlpha * accel.acceleration.y + (1 - filterAlpha) * filteredY;
  filteredZ = filterAlpha * accel.acceleration.z + (1 - filterAlpha) * filteredZ;

  // Проверяем стабильность показаний
  static bool isStable = false;
  static uint32_t lastStableTime = 0;
  static int currentOrientation = 0;
  
  float deltaX = abs(accel.acceleration.x - filteredX);
  float deltaY = abs(accel.acceleration.y - filteredY);
  float deltaZ = abs(accel.acceleration.z - filteredZ);

  if (deltaX < STABLE_THRESHOLD && deltaY < STABLE_THRESHOLD && deltaZ < STABLE_THRESHOLD) {
    if (!isStable) {
      lastStableTime = millis();
      isStable = true;
    }
    
    // Ждем 500 мс стабильных показаний перед определением ориентации
    if (millis() - lastStableTime > 500) {
      int newOrientation = determineOrientation(filteredX, filteredY, filteredZ);
      if (newOrientation != currentOrientation) {
        currentOrientation = newOrientation;
        tft.setRotation(currentOrientation);
        tft.fillScreen(GC9A01A_BLACK);
        drawCenteredText();
      }
    }
  } else {
    isStable = false;
  }

  delay(50);
}

int determineOrientation(float x, float y, float z) {
  // Определяем доминирующую ось
  if (fabs(x) > fabs(y)) {
    if (x > ORIENTATION_THRESHOLD) return 3;  // Поворот влево (было 1)
    if (x < -ORIENTATION_THRESHOLD) return 1; // Поворот вправо (было 3)
  } else {
    if (y > ORIENTATION_THRESHOLD) return 2;  // Поворот вниз
    if (y < -ORIENTATION_THRESHOLD) return 0; // Поворот вверх
  }
  return 0;  // По умолчанию
}

void drawCenteredText() {
  int16_t x, y;
  uint16_t w, h;
  tft.getTextBounds(TEXT, 0, 0, &x, &y, &w, &h);
  
  switch(tft.getRotation()) {
    case 0:
    case 2:
      tft.setCursor(CENTER_X - w/2, CENTER_Y - h/2);
      break;
    case 1:
    case 3:
      tft.setCursor(CENTER_Y - h/2, CENTER_X - w/2);
      break;
  }
  tft.print(TEXT);
}
