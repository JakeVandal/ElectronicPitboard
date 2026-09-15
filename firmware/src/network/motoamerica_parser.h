#pragma once

#include <Arduino.h>
#include <cstddef>
#include <string>

#include "display_types.h"

namespace pitboard {

class MotoAmericaParser {
 public:
  static constexpr size_t kMaxActiveRoster = 64;

  MotoAmericaParser();
  void setTrackedRider(uint8_t riderNumber);
  uint8_t trackedRiderNumber() const;
  const RiderInfo* activeRoster() const;
  size_t activeRosterCount() const;
  bool rosterDirty() const;
  void clearRosterDirty();

  uint32_t computeRefreshPeriodMs(uint32_t serverLoad) const;
  bool parseChunk(const char* chunk, size_t length, TelemetryFrame& frame, bool& frameValid);
  bool parseCompleteBuffer(const char* buffer, size_t length, TelemetryFrame& frame, bool& frameValid);

 private:
  bool parseActiveRoster(const std::string& text);

  uint8_t trackedRiderNumber_ = 1;
  RiderInfo activeRoster_[kMaxActiveRoster] = {};
  size_t activeRosterCount_ = 0;
  bool rosterDirty_ = false;
};

}  // namespace pitboard
