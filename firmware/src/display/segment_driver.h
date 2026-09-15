#pragma once

#include <Arduino.h>
#include "display_types.h"

namespace pitboard {

class SegmentDriver {
 public:
  SegmentDriver();
  void begin();
  void update(const SegmentLine& line);
  void clear();
  void setBrightness(uint8_t brightness);

 private:
  uint8_t brightness_ = 255;
};

}  // namespace pitboard
