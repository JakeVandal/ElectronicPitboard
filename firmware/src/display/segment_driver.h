#pragma once

#include <Arduino.h>
#include <array>

#include "display_types.h"

namespace pitboard {

class SegmentDriver {
 public:
  SegmentDriver();
  void begin();
  void update(const SegmentLine& line);
  void clear();
  void setBrightness(uint8_t brightness);
  void runStartupTest();
  void setRow1_LapTime(uint8_t min, uint8_t sec, uint8_t hundredths);
  void setRow2_Position(uint8_t position, uint8_t lap);
  void setRow3_Gap(float gapSeconds);
  void setRow3_Message(const char* text);

 private:
  uint8_t brightness_ = 200;
  std::array<std::array<char, 18>, 3> rowBuffer_ = {{{0}}};

  static uint8_t encodeCharacter(char ch);
  static uint8_t encodeDigit(uint8_t digit);
  void writeShiftRegister(uint8_t value);
  void flushRows();
};

}  // namespace pitboard
