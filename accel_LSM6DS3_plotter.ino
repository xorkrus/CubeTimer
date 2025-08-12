#include <Wire.h>
#include <Adafruit_LSM6DS3.h>

Adafruit_LSM6DS3 lsm6ds3;

void setup() {
  Serial.begin(115200);
  while (!Serial); // Ожидание открытия Serial-порта

  // Инициализация I2C (пины 0 и 1 на RP2040)
  Wire.setSDA(0);
  Wire.setSCL(1);
  Wire.begin();

  if (!lsm6ds3.begin_I2C(0x6B)) { // Адрес 0x6B
    Serial.println("Ошибка! LSM6DS3 не найден.");
    while (1);
  }

  Serial.println("LSM6DS3 найден!");

  // Настройка акселерометра (пример: ±4g и 104 Гц)
  lsm6ds3.setAccelRange(LSM6DS_ACCEL_RANGE_4_G);
  lsm6ds3.setAccelDataRate(LSM6DS_RATE_104_HZ);
}

void loop() {
  sensors_event_t accel;
  lsm6ds3.getEvent(&accel, nullptr, nullptr); // Получаем только данные акселерометра

  // Вывод в формате для плоттера (X, Y, Z в g)
  Serial.print(accel.acceleration.x);
  Serial.print(",");
  Serial.print(accel.acceleration.y);
  Serial.print(",");
  Serial.println(accel.acceleration.z);

  delay(100); // Задержка для стабильности
}
