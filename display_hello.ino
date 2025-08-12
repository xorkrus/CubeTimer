#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_GC9A01A.h>

// Конфигурация пинов
#define TFT_CS   4  // Chip Select
#define TFT_DC   6  // Data/Command
#define TFT_RST  8  // Reset
#define TFT_SCK  5  // SPI Clock
#define TFT_MOSI 7  // SPI Data

// Инициализация дисплея
Adafruit_GC9A01A tft(TFT_CS, TFT_DC, TFT_MOSI, TFT_SCK, TFT_RST);

// Параметры круга
#define CENTER_X 120
#define CENTER_Y 120
#define RADIUS   119

void setup() {
  // Инициализация дисплея
  tft.begin();
  tft.setRotation(0);  // Ориентация (0-3)
  tft.fillScreen(GC9A01A_BLACK);  // Очистка экрана

  // Настройка текста
  tft.setTextColor(GC9A01A_WHITE);  // Цвет текста
  tft.setTextSize(2);               // Размер текста (1-4)
  tft.setTextWrap(false);           // Отключить перенос текста

  // Вывод "Hello" в центре
  int16_t x, y;
  uint16_t w, h;
  tft.getTextBounds("Hello", 0, 0, &x, &y, &w, &h);  // Получить размеры текста
  tft.setCursor(CENTER_X - w/2, CENTER_Y - h/2);     // Позиционирование по центру
  tft.print("Hello");                                // Вывод текста
}

void loop() {
  // Пустой цикл (можете добавить свой код)
}
