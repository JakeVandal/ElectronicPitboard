#pragma once

#include <Arduino.h>

namespace pitboard {

constexpr uint8_t kDisplayRows = 3;
constexpr uint8_t kDigitsPerRow = 6;
constexpr uint8_t kTotalDigits = kDisplayRows * kDigitsPerRow;

constexpr uint8_t kPinSegData = 11;
constexpr uint8_t kPinSegClk = 12;
constexpr uint8_t kPinSegLatch = 10;
constexpr uint8_t kPinSegOePwm = 9;

constexpr uint8_t kPinTftMosi = 13;
constexpr uint8_t kPinTftSclk = 14;
constexpr uint8_t kPinTftCs = 15;
constexpr uint8_t kPinTftDc = 2;
constexpr uint8_t kPinTftRst = 4;
constexpr uint8_t kPinTftBl = 5;

constexpr uint8_t kPinTouchSda = 17;
constexpr uint8_t kPinTouchScl = 18;
constexpr uint8_t kPinTouchInt = 16;
constexpr uint8_t kPinTouchRst = 21;

constexpr uint8_t kPinTelemTx = 43;
constexpr uint8_t kPinTelemRx = 44;
constexpr uint8_t kPinTelemReset = 46;
constexpr uint8_t kPinStatusLed = 48;
constexpr uint8_t kPinPowerSense = 45;

constexpr uint8_t kSegmentClkPin = kPinSegClk;
constexpr uint8_t kSegmentLatchPin = kPinSegLatch;
constexpr uint8_t kSegmentDataPin = kPinSegData;
constexpr uint8_t kDisplayBlankPin = kPinSegOePwm;

constexpr uint8_t kHoysundTftCsPin = kPinTftCs;
constexpr uint8_t kHoysundTftDcPin = kPinTftDc;
constexpr uint8_t kHoysundTftRstPin = kPinTftRst;
constexpr uint8_t kHoysundTouchIrqPin = kPinTouchInt;

constexpr uint8_t kSecondaryLcdTxPin = kPinTelemTx;
constexpr uint8_t kSecondaryLcdRxPin = kPinTelemRx;
constexpr uint8_t kSecondaryLcdResetPin = kPinTelemReset;

constexpr uint32_t kWifiConnectTimeoutMs = 20000;
constexpr uint32_t kMotoAmericaPollMs = 1000;
constexpr uint32_t kBleHeartbeatMs = 5000;
constexpr uint32_t kServerLoadThresholdLow = 10;
constexpr uint32_t kServerLoadThresholdMid = 50;

constexpr uint8_t kMaxSegmentBuffer = 144;
constexpr uint8_t kUsbProgTx = 20;
constexpr uint8_t kUsbProgRx = 21;

constexpr char kWifiSsid[] = "PITBOARD_WLAN";
constexpr char kWifiPassword[] = "change-me";
constexpr char kMotoAmericaUrl[] = "http://timing.motoamerica.com";
constexpr char kMotoAmericaPollingEndpoint[] = "/timing_frame.php";

constexpr char kBleServiceUuid[] = "0000FFB0-0000-1000-8000-00805F9B34FB";
constexpr char kBleRow1CharUuid[] = "0000FFB1-0000-1000-8000-00805F9B34FB";
constexpr char kBleRow2CharUuid[] = "0000FFB2-0000-1000-8000-00805F9B34FB";
constexpr char kBleRow3CharUuid[] = "0000FFB3-0000-1000-8000-00805F9B34FB";
constexpr char kBleControlCharUuid[] = "0000FFB4-0000-1000-8000-00805F9B34FB";
constexpr char kBleStatusCharUuid[] = "0000FFB5-0000-1000-8000-00805F9B34FB";
constexpr char kBleControlUuid[] = "0000FFB4-0000-1000-8000-00805F9B34FB";
constexpr char kBleStatusUuid[] = "0000FFB5-0000-1000-8000-00805F9B34FB";

}  // namespace pitboard
