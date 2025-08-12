#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_GC9A01A.h>

#define TFT_CS   4
#define TFT_DC   6
#define TFT_RST  8
#define TFT_SCK  5
#define TFT_MOSI 7

Adafruit_GC9A01A tft(TFT_CS, TFT_DC, TFT_MOSI, TFT_SCK, TFT_RST);

#define CENTER_X 120
#define CENTER_Y 120
#define RADIUS 119

// Цвета полос
const uint16_t colors[8] = {
  GC9A01A_RED, GC9A01A_ORANGE, GC9A01A_YELLOW, GC9A01A_GREEN,
  GC9A01A_CYAN, GC9A01A_BLUE, GC9A01A_MAGENTA, GC9A01A_WHITE
};

void setup() {
  tft.begin();
  tft.setRotation(0);
  tft.fillScreen(GC9A01A_BLACK);
}

void drawStripes(int angle) {
  int stripeWidth = 240 / 8;
  
  for (int i = 0; i < 8; i++) {
    int x1 = i * stripeWidth;
    int x2 = x1 + stripeWidth;
    
    // Быстрая заливка прямоугольников без проверки круга
    tft.fillRect(x1, 0, stripeWidth, 240, colors[i]);
  }
  
  // Обрезаем по кругу в конце
  for (int y = 0; y < 240; y++) {
    for (int x = 0; x < 240; x++) {
      int dx = x - CENTER_X;
      int dy = y - CENTER_Y;
      if (dx*dx + dy*dy > RADIUS*RADIUS) {
        tft.drawPixel(x, y, GC9A01A_BLACK);
      }
    }
  }
}

void loop() {
  // 4 прохода с разными углами поворота
  for (int rotation = 0; rotation < 4; rotation++) {
    tft.setRotation(rotation);
    
    // Быстрая заливка полосами
    unsigned long start = millis();
    while (millis() - start < 4000) { // 4 секунды на каждую ориентацию
      drawStripes(rotation * 45);
    }
  }
}
