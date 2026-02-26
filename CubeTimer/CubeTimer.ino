/**
 * Проект: Умный куб на Waveshare RP2040-Zero
 * Задача 1: Определение стороны куба и её фиксация
 * 
 * Компоненты: LSM6DS3 (акселерометр/гироскоп) по I2C: SDA=GP0, SCL=GP1
 * Библиотеки: Adafruit LSM6DS, Adafruit Unified Sensor
 * 
 * Сценарий: куб стоит на столе → берут в руки → переворачивают → ставят на стол
 * Результат: в терминал выводится строка вида "[СТОРОНА 1 | x | y | z ]"
 * после фиксации (когда координаты перестают сильно меняться).
 * 
 * Подключение:
 * - LSM6DS3 VCC -> 3.3V
 * - LSM6DS3 GND -> GND
 * - LSM6DS3 SDA -> GPIO0
 * - LSM6DS3 SCL -> GPIO1
 */

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_LSM6DS.h>   // Универсальная библиотека для LSM6DS3

// Пины I2C для RP2040-Zero
#define I2C_SDA  0
#define I2C_SCL  1

// Порог изменения ускорения (м/с²) для определения стабильности
const float STABILITY_THRESHOLD = 0.2;
// Количество последовательных стабильных измерений для фиксации
const int STABLE_COUNT_REQUIRED = 10;

// Объект датчика
Adafruit_LSM6DS lsm6ds;

// Переменные для отслеживания стабильности
float prevAx, prevAy, prevAz;   // Предыдущие значения
int stableCount = 0;             // Счётчик стабильных итераций
bool lastStableState = false;    // Было ли состояние стабильным на прошлой итерации

/**
 * Определение стороны куба по данным акселерометра.
 * Возвращает строку с номером стороны от 1 до 6:
 * 1: X+   2: X-   3: Y+   4: Y-   5: Z+   6: Z-
 */
String getSide(float ax, float ay, float az) {
  float absX = fabs(ax);
  float absY = fabs(ay);
  float absZ = fabs(az);

  if (absX > absY && absX > absZ) {
    return (ax > 0) ? "1" : "2";   // X+ или X-
  } else if (absY > absX && absY > absZ) {
    return (ay > 0) ? "3" : "4";   // Y+ или Y-
  } else {
    return (az > 0) ? "5" : "6";   // Z+ или Z-
  }
}

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("Запуск детектора стороны куба...");

  // Настройка пинов I2C для RP2040
  Wire.setSDA(I2C_SDA);
  Wire.setSCL(I2C_SCL);
  Wire.begin();

  // Инициализация LSM6DS3
  if (!lsm6ds.begin_I2C()) {
    Serial.println("Ошибка инициализации LSM6DS3!");
    while (1) delay(10);
  }
  Serial.println("LSM6DS3 найден!");

  // Настройка диапазона акселерометра (2G достаточно для определения гравитации)
  lsm6ds.setAccelRange(LSM6DS_ACCEL_RANGE_2_G);
  // Частота измерений 104 Гц
  lsm6ds.setAccelDataRate(LSM6DS_RATE_104_HZ);

  // Первое чтение для инициализации предыдущих значений
  sensors_event_t accel, gyro, temp;
  lsm6ds.getEvent(&accel, &gyro, &temp);
  prevAx = accel.acceleration.x;
  prevAy = accel.acceleration.y;
  prevAz = accel.acceleration.z;

  Serial.println("Готов. Ожидание стабильности...");
}

void loop() {
  // Чтение данных с датчика
  sensors_event_t accel, gyro, temp;
  lsm6ds.getEvent(&accel, &gyro, &temp);

  float ax = accel.acceleration.x;
  float ay = accel.acceleration.y;
  float az = accel.acceleration.z;

  // Вычисляем изменения относительно предыдущего измерения
  float diffX = fabs(ax - prevAx);
  float diffY = fabs(ay - prevAy);
  float diffZ = fabs(az - prevAz);

  // Проверка стабильности
  bool currentlyStable = false;
  if (diffX < STABILITY_THRESHOLD && diffY < STABILITY_THRESHOLD && diffZ < STABILITY_THRESHOLD) {
    stableCount++;
    if (stableCount >= STABLE_COUNT_REQUIRED) {
      currentlyStable = true;
    }
  } else {
    stableCount = 0;          // Движение обнаружено – сброс счётчика
    currentlyStable = false;
  }

  // Если произошёл переход от нестабильного к стабильному – фиксируем сторону
  if (currentlyStable && !lastStableState) {
    String side = getSide(ax, ay, az);
    Serial.print("[СТОРОНА ");
    Serial.print(side);
    Serial.print(" | ");
    Serial.print(ax);
    Serial.print(" | ");
    Serial.print(ay);
    Serial.print(" | ");
    Serial.print(az);
    Serial.println(" ]");
  }

  // Обновляем состояние для следующей итерации
  lastStableState = currentlyStable;
  prevAx = ax;
  prevAy = ay;
  prevAz = az;

  // Небольшая задержка для стабильности цикла (~100 Гц)
  delay(10);
}
