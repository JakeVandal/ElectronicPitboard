#pragma once

#include <Arduino.h>

namespace pitboard {

constexpr uint8_t kDisplayRows = 3;
constexpr uint8_t kDigitsPerRow = 6;
constexpr uint8_t kTotalDigits = kDisplayRows * kDigitsPerRow;

constexpr uint8_t kSegmentClkPin = 4;
constexpr uint8_t kSegmentLatchPin = 5;
constexpr uint8_t kSegmentDataPin = 6;
constexpr uint8_t kDisplayBlankPin = 7;

constexpr uint8_t kHoysundTftCsPin = 10;
constexpr uint8_t kHoysundTftDcPin = 11;
constexpr uint8_t kHoysundTftRstPin = 12;
constexpr uint8_t kHoysundTouchIrqPin = 13;

constexpr uint8_t kSecondaryLcdTxPin = 17;
constexpr uint8_t kSecondaryLcdRxPin = 18;
constexpr uint8_t kSecondaryLcdResetPin = 19;

constexpr uint8_t kStatusLedPin = 48;
constexpr uint8_t kPowerSensePin = 45;

constexpr uint32_t kWifiConnectTimeoutMs = 20000;
constexpr uint32_t kMotoAmericaPollMs = 1000;
constexpr uint32_t kBleHeartbeatMs = 5000;

constexpr char kWifiSsid[] = "PITBOARD_WLAN";
constexpr char kWifiPassword[] = "change-me";

constexpr char kBleServiceUuid[] = "6b6f-7462-6f61-7264-0001";
constexpr char kBleControlUuid[] = "6b6f-7462-6f61-7264-0002";
constexpr char kBleStatusUuid[] = "6b6f-7462-6f61-7264-0003";

constexpr char kMotoAmericaUrl[] = "http://192.168.1.50/timing_frame.php";

}  // namespace pitboard
