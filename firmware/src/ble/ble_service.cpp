#include "ble/ble_service.h"

#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>

#include "config.h"

namespace pitboard {

namespace {
class PitboardCallbacks : public BLEServerCallbacks {
 public:
  void onConnect(BLEServer* server) override {
    (void)server;
  }

  void onDisconnect(BLEServer* server) override {
    (void)server;
  }
};

}  // namespace

BleService::BleService() = default;

void BleService::begin() {
  BLEDevice::init("ESP32 Smart Pitboard");
  auto* server = BLEDevice::createServer();
  server->setCallbacks(new PitboardCallbacks());
  auto* service = server->createService(BLEUUID(kBleServiceUuid));
  auto* status = service->createCharacteristic(
      BLEUUID(kBleStatusUuid),
      BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_NOTIFY);
  auto* control = service->createCharacteristic(
      BLEUUID(kBleControlUuid),
      BLECharacteristic::PROPERTY_WRITE | BLECharacteristic::PROPERTY_READ);

  status->setValue("READY");
  control->setValue("HELLO");
  service->start();
  BLEAdvertising* advertising = BLEDevice::getAdvertising();
  advertising->addServiceUUID(BLEUUID(kBleServiceUuid));
  advertising->start();
}

bool BleService::isConnected() const {
  return false;
}

void BleService::updateStatus(const char* status) {
  (void)status;
}

}  // namespace pitboard
