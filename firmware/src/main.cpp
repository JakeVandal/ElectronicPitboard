#include <Arduino.h>
#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>

#include "config.h"
#include "display_types.h"

namespace {

QueueHandle_t g_displayQueue = nullptr;

void initializeQueues() {
  g_displayQueue = xQueueCreate(8, sizeof(pitboard::DisplayCommand));
  if (g_displayQueue == nullptr) {
    Serial.println("Failed to create display queue");
  }
}

void initializeSerial() {
  Serial.begin(115200);
  delay(500);
  Serial.println("ESP32-S3 Smart Pitboard booting");
}

void runDisplayTask(void *parameter) {
  (void)parameter;

  pitboard::DisplayCommand cmd;
  while (true) {
    if (xQueueReceive(g_displayQueue, &cmd, portMAX_DELAY) == pdTRUE) {
      Serial.printf("Display command: %u\n", static_cast<uint8_t>(cmd.type));
    }
    vTaskDelay(pdMS_TO_TICKS(10));
  }
}

}  // namespace

void setup() {
  initializeSerial();
  initializeQueues();

  xTaskCreatePinnedToCore(runDisplayTask, "display_task", 4096, nullptr, 2, nullptr, 1);
}

void loop() {
  static uint32_t lastHeartbeat = 0;
  const uint32_t now = millis();

  if (now - lastHeartbeat > pitboard::kBleHeartbeatMs) {
    lastHeartbeat = now;
    Serial.println("Runtime heartbeat");
  }

  delay(50);
}
