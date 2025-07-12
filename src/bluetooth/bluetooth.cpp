#include "bluetooth.h"
#include "../keyboard/keyboard.h"
#include "BLEDevice.h"
#include <M5Unified.h>

#ifndef DEVICE_BLUETOOTH_NAME
#define DEVICE_BLUETOOTH_NAME "Handy Mini"
#endif

#define DEVICE_MANUFACTURER_NAME "M5Stack"

bool isBleConnected = false;
BLEHIDDevice *hid = nullptr;
BLECharacteristic *input;
BLECharacteristic *output;

const uint8_t REPORT_MAP[] = {
    USAGE_PAGE(1),      0x01, USAGE(1),           0x06,
    COLLECTION(1),      0x01, REPORT_ID(1),       0x01,
    USAGE_PAGE(1),      0x07, USAGE_MINIMUM(1),   0xE0,
    USAGE_MAXIMUM(1),   0xE7, LOGICAL_MINIMUM(1), 0x00,
    LOGICAL_MAXIMUM(1), 0x01, REPORT_COUNT(1),    0x08,
    REPORT_SIZE(1),     0x01, HIDINPUT(1),        0x02,
    REPORT_COUNT(1),    0x01, REPORT_SIZE(1),     0x08,
    HIDINPUT(1),        0x01, REPORT_COUNT(1),    0x06,
    REPORT_SIZE(1),     0x08, LOGICAL_MINIMUM(1), 0x00,
    LOGICAL_MAXIMUM(1), 0x65, USAGE_MINIMUM(1),   0x00,
    USAGE_MAXIMUM(1),   0x65, HIDINPUT(1),        0x00,
    REPORT_COUNT(1),    0x05, REPORT_SIZE(1),     0x01,
    USAGE_PAGE(1),      0x08, USAGE_MINIMUM(1),   0x01,
    USAGE_MAXIMUM(1),   0x05, LOGICAL_MINIMUM(1), 0x00,
    LOGICAL_MAXIMUM(1), 0x01, HIDOUTPUT(1),       0x02,
    REPORT_COUNT(1),    0x01, REPORT_SIZE(1),     0x03,
    HIDOUTPUT(1),       0x01, END_COLLECTION(0)};

void BleKeyboardCallbacks::onConnect(BLEServer *server) {
  isBleConnected = true;

  BLE2902 *cccDesc =
      (BLE2902 *)input->getDescriptorByUUID(BLEUUID((uint16_t)0x2902));
  cccDesc->setNotifications(true);

  M5_LOGI("Client has connected");
}

void BleKeyboardCallbacks::onDisconnect(BLEServer *server) {
  isBleConnected = false;

  BLE2902 *cccDesc =
      (BLE2902 *)input->getDescriptorByUUID(BLEUUID((uint16_t)0x2902));
  cccDesc->setNotifications(false);

  M5_LOGI("Client has disconnected");
}

void OutputCallbacks::onWrite(BLECharacteristic *characteristic) {
  OutputReport *report = (OutputReport *)characteristic->getData();
}

void bluetoothTask(void *) {
  BLEDevice::init(DEVICE_BLUETOOTH_NAME);
  BLEServer *server = BLEDevice::createServer();
  server->setCallbacks(new BleKeyboardCallbacks());

  hid = new BLEHIDDevice(server);
  input = hid->inputReport(1);
  output = hid->outputReport(1);
  output->setCallbacks(new OutputCallbacks());

  hid->manufacturer()->setValue(DEVICE_MANUFACTURER_NAME);
  hid->pnp(0x02, 0xe502, 0xa111, 0x0210);
  hid->hidInfo(0x00, 0x02);

  BLESecurity *security = new BLESecurity();
  security->setAuthenticationMode(ESP_LE_AUTH_BOND);

  hid->reportMap((uint8_t *)REPORT_MAP, sizeof(REPORT_MAP));
  hid->startServices();

  BLEAdvertising *advertising = server->getAdvertising();
  advertising->setAppearance(HID_KEYBOARD);
  advertising->addServiceUUID(hid->hidService()->getUUID());
  advertising->addServiceUUID(hid->deviceInfo()->getUUID());
  advertising->start();

  M5_LOGI("BLE Ready.");
  delay(portMAX_DELAY);
}
