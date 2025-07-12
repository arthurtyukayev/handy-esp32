#pragma once

#include "BLEDevice.h"
#include "BLEHIDDevice.h"
#include <Arduino.h>

extern bool isBleConnected;
extern BLEHIDDevice *hid;
extern const uint8_t REPORT_MAP[];

void bluetoothTask(void *);

class BleKeyboardCallbacks : public BLEServerCallbacks {
  void onConnect(BLEServer *server) override;
  void onDisconnect(BLEServer *server) override;
};

class OutputCallbacks : public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic *characteristic) override;
};
