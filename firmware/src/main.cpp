#include <Arduino.h>
#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>
#include <freertos/semphr.h>

#include "config.h"
#include "display_types.h"

namespace {

QueueHandle_t g_segmentQueue = nullptr;
QueueHandle_t g_tftQueue = nullptr;
QueueHandle_t g_networkQueue = nullptr;
QueueHandle_t g_bleQueue = nullptr;
SemaphoreHandle_t g_displayMutex = nullptr;

void initializeQueues() {
  g_segmentQueue = xQueueCreate(8, sizeof(pitboard::DisplayCommand));
  g_tftQueue = xQueueCreate(8, sizeof(pitboard::DisplayCommand));
  g_networkQueue = xQueueCreate(8, sizeof(pitboard::TelemetryFrame));
  g_bleQueue = xQueueCreate(4, sizeof(pitboard::DisplayCommand));
  g_displayMutex = xSemaphoreCreateMutex();

  if (g_segmentQueue == nullptr || g_tftQueue == nullptr || g_networkQueue == nullptr ||
      g_bleQueue == nullptr || g_displayMutex == nullptr) {
    Serial.println("Failed to initialize FreeRTOS resources");
  }
}

void initializePins() {
  pinMode(pitboard::kPinStatusLed, OUTPUT);
  pinMode(pitboard::kPinPowerSense, INPUT);
  pinMode(pitboard::kPinTftBl, OUTPUT);
  pinMode(pitboard::kPinSegOePwm, OUTPUT);

  digitalWrite(pitboard::kPinStatusLed, HIGH);
  analogWrite(pitboard::kPinSegOePwm, 200);
  analogWrite(pitboard::kPinTftBl, 180);
}

void initializeSerial() {
  Serial.begin(115200);
  delay(250);
  Serial.println("ESP32-S3 Smart Pitboard booting");
}

void taskSegments(void *parameter) {
  (void)parameter;
  pitboard::DisplayCommand command;
  while (true) {
    if (xQueueReceive(g_segmentQueue, &command, portMAX_DELAY) == pdTRUE) {
      if (xSemaphoreTake(g_displayMutex, portMAX_DELAY) == pdTRUE) {
        Serial.printf("TaskSegments: mode=%u text=%s\n", static_cast<uint8_t>(command.status.mode), command.line.text);
        xSemaphoreGive(g_displayMutex);
      }
    }
    vTaskDelay(pdMS_TO_TICKS(10));
  }
}

void taskTftUi(void *parameter) {
  (void)parameter;
  pitboard::DisplayCommand command;
  while (true) {
    if (xQueueReceive(g_tftQueue, &command, portMAX_DELAY) == pdTRUE) {
      Serial.printf("TaskTFT_UI: live=%d ble=%d\n", command.status.wifiConnected, command.status.bleConnected);
    }
    vTaskDelay(pdMS_TO_TICKS(15));
  }
}

void taskNetwork(void *parameter) {
  (void)parameter;
  pitboard::TelemetryFrame telemetry;
  while (true) {
    if (xQueueReceive(g_networkQueue, &telemetry, pdMS_TO_TICKS(1000)) == pdTRUE) {
      pitboard::DisplayCommand command;
      command.type = pitboard::MessageType::kSetTelemetry;
      command.telemetry = telemetry;
      xQueueSend(g_segmentQueue, &command, portMAX_DELAY);
      xQueueSend(g_tftQueue, &command, portMAX_DELAY);
    }
    vTaskDelay(pdMS_TO_TICKS(50));
  }
}

void taskBle(void *parameter) {
  (void)parameter;
  pitboard::DisplayCommand command;
  while (true) {
    if (xQueueReceive(g_bleQueue, &command, pdMS_TO_TICKS(250)) == pdTRUE) {
      Serial.printf("TaskBLE: command=%u\n", static_cast<uint8_t>(command.type));
    }
    vTaskDelay(pdMS_TO_TICKS(20));
  }
}

}  // namespace

void setup() {
  initializeSerial();
  initializePins();
  initializeQueues();

  xTaskCreatePinnedToCore(taskSegments, "TaskSegments", 4096, nullptr, 3, nullptr, 1);
  xTaskCreatePinnedToCore(taskTftUi, "TaskTFT_UI", 4096, nullptr, 2, nullptr, 1);
  xTaskCreatePinnedToCore(taskNetwork, "TaskNetwork", 4096, nullptr, 2, nullptr, 0);
  xTaskCreatePinnedToCore(taskBle, "TaskBLE", 4096, nullptr, 1, nullptr, 0);

  Serial.println("All FreeRTOS tasks started");
}

void loop() {
  static uint32_t lastRuntime = 0;
  const uint32_t now = millis();
  if (now - lastRuntime > pitboard::kBleHeartbeatMs) {
    lastRuntime = now;
    digitalWrite(pitboard::kPinStatusLed, !digitalRead(pitboard::kPinStatusLed));
  }

  delay(25);
}
