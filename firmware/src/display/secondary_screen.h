#pragma once

#include <Arduino.h>

namespace pitboard {

class SecondaryScreen {
 public:
  SecondaryScreen();
  void begin();
  void updateTelemetry(float speedMph, float rpm, int position, int lap);
};

}  // namespace pitboard
