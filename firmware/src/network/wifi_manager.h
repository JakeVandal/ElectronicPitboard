#pragma once

#include <Arduino.h>

namespace pitboard {

class WifiManager {
 public:
  WifiManager();
  void begin();
  bool connect();
  bool isConnected() const;

 private:
  bool connected_ = false;
};

}  // namespace pitboard
