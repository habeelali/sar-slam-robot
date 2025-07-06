#include <Arduino.h>
#include <Wire.h>
#include <MPU6050.h>

#define SDA_PIN 18
#define SCL_PIN 17

MPU6050 mpu;

// RTOS task handle
TaskHandle_t imuTaskHandle = NULL;

void printMemoryInfo() {
  uint32_t flashSize = ESP.getFlashChipSize();
  uint32_t psramSize = ESP.getPsramSize();
  Serial.printf("Flash: %u bytes (%.2f MB)\n", flashSize, flashSize / 1048576.0);
  Serial.printf("PSRAM: %u bytes (%.2f MB)\n", psramSize, psramSize / 1048576.0);
}

// IMU reading and printing task
void imuTask(void *pvParameters) {
  int16_t ax, ay, az, gx, gy, gz;
  while (1) {
    mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);

    Serial.printf("\rRAW: AX:%6d AY:%6d AZ:%6d  GX:%6d GY:%6d GZ:%6d   ", ax, ay, az, gx, gy, gz);
    Serial.printf(" | IMU: %d,%d,%d,%d,%d,%d   ", ax, ay, az, gx, gy, gz);

    vTaskDelay(pdMS_TO_TICKS(10)); // ~20Hz update rate
  }
}

void setup() {
  Serial.begin(115200);
  delay(1000);

  printMemoryInfo();

  Wire.begin(SDA_PIN, SCL_PIN);
  mpu.initialize();

  if (!mpu.testConnection()) {
    Serial.println("MPU6050 connection failed!");
    while (1) delay(100);
  } else {
    Serial.println("MPU6050 connected.");
  }
  delay(1000);

  // Create the IMU RTOS task
  xTaskCreatePinnedToCore(
    imuTask,           // Task function
    "IMU Task",        // Name
    4096,              // Stack size (bytes)
    NULL,              // Parameter
    1,                 // Priority
    &imuTaskHandle,    // Task handle
    1                  // Core (0 or 1)
  );
}

void loop() {
  // Main loop can remain empty or handle other tasks
}
