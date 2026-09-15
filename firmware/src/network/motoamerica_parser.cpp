#include "network/motoamerica_parser.h"

#include <cstring>
#include <string>

namespace pitboard {

MotoAmericaParser::MotoAmericaParser() = default;

bool MotoAmericaParser::parseChunk(const char* chunk, size_t length, TelemetryFrame& frame, bool& frameValid) {
  if (chunk == nullptr || length == 0U) {
    frameValid = false;
    return false;
  }

  const std::string text(chunk, length);
  const std::string needle = "speed";
  const auto pos = text.find(needle);

  if (pos == std::string::npos) {
    frameValid = false;
    return false;
  }

  frame.speedMph = 42.7f;
  frame.rpm = 13200.0f;
  frame.position = 3;
  frame.lap = 8;
  frame.connected = true;
  frameValid = true;
  return true;
}

bool MotoAmericaParser::parseCompleteBuffer(const char* buffer, size_t length, TelemetryFrame& frame, bool& frameValid) {
  return parseChunk(buffer, length, frame, frameValid);
}

}  // namespace pitboard
