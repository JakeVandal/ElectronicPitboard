#include "network/wifi_manager.h"

#include <WiFi.h>

#include "config.h"

namespace pitboard {

WifiManager::WifiManager() = default;

void WifiManager::begin() {
  WiFi.mode(WIFI_STA);
  WiFi.setAutoReconnect(true);
}

bool WifiManager::connect() {
  if (WiFi.status() == WL_CONNECTED) {
    connected_ = true;
    return true;
  }

  WiFi.begin(kWifiSsid, kWifiPassword);
  const uint32_t start = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - start < kWifiConnectTimeoutMs) {
    delay(100);
  }

  connected_ = (WiFi.status() == WL_CONNECTED);
  return connected_;
}

bool WifiManager::isConnected() const {
  return connected_ || (WiFi.status() == WL_CONNECTED);
}

}  // namespace pitboard
