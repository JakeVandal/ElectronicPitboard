#pragma once

#include <Arduino.h>

namespace pitboard {

class BleService {
 public:
  BleService();
  void begin();
  bool isConnected() const;
  void updateStatus(const char* status);
};

}  // namespace pitboard
