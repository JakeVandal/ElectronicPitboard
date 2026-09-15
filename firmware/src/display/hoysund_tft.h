#pragma once

#include <Arduino.h>

namespace pitboard {

class HoysundTft {
 public:
  HoysundTft();
  void begin();
  void renderStatus(const char* text);
  void renderQr(const char* payload);
};

}  // namespace pitboard
