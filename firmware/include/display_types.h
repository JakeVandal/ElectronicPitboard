#pragma once

#include <Arduino.h>

namespace pitboard {

enum class DisplayMode : uint8_t {
  kManual = 0,
  kLiveTiming = 1,
  kStandby = 2,
  kFault = 3,
};

enum class MessageType : uint8_t {
  kNoop = 0,
  kSetText = 1,
  kSetMode = 2,
  kSetTelemetry = 3,
  kSetStatus = 4,
};

struct SegmentLine {
  char text[18];
  bool blink = false;
  uint8_t brightness = 255;
};

struct TelemetryFrame {
  int lap = 0;
  int position = 0;
  float speedMph = 0.0f;
  float rpm = 0.0f;
  float deltaSeconds = 0.0f;
  float lastLapSeconds = 0.0f;
  bool connected = false;
  uint8_t riderNumber = 0;
  char lastLapText[16] = {0};
};

struct PitStatus {
  DisplayMode mode = DisplayMode::kStandby;
  bool wifiConnected = false;
  bool bleConnected = false;
  bool displayActive = true;
  uint8_t batteryPercent = 100;
  int rssiDbm = -90;
};

struct DisplayCommand {
  MessageType type = MessageType::kNoop;
  SegmentLine line;
  TelemetryFrame telemetry;
  PitStatus status;
};

struct RiderInfo {
  uint8_t bikeNumber = 0;
  char riderName[32] = "";
  int position = 0;
  char lastLapText[16] = "";
};

struct SessionInfo {
  char trackName[32] = "Road Atlanta";
  char sessionType[16] = "Race";
  uint32_t elapsedMs = 0;
  uint8_t riderCount = 0;
};

}  // namespace pitboard
