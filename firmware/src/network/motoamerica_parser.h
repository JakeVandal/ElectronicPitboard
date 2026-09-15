#pragma once

#include <Arduino.h>

#include "display_types.h"

namespace pitboard {

class MotoAmericaParser {
 public:
  MotoAmericaParser();
  bool parseChunk(const char* chunk, size_t length, TelemetryFrame& frame, bool& frameValid);
  bool parseCompleteBuffer(const char* buffer, size_t length, TelemetryFrame& frame, bool& frameValid);
};

}  // namespace pitboard
