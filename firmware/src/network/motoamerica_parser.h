#pragma once

#include <Arduino.h>
#include <cstddef>

#include "display_types.h"

namespace pitboard {

class MotoAmericaParser {
 public:
  MotoAmericaParser();
  void setTrackedRider(uint8_t riderNumber);
  uint32_t computeRefreshPeriodMs(uint32_t serverLoad) const;
  bool parseChunk(const char* chunk, size_t length, TelemetryFrame& frame, bool& frameValid);
  bool parseCompleteBuffer(const char* buffer, size_t length, TelemetryFrame& frame, bool& frameValid);

 private:
  uint8_t trackedRiderNumber_ = 1;
};

}  // namespace pitboard
