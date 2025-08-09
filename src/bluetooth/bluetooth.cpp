#include "bluetooth.h"
#include "../battery/battery.h"
#include "../keyboard/keyboard.h"
#include "BLEDevice.h"
#include "shared.h"
#include <M5Unified.h>

#ifndef DEVICE_BLUETOOTH_NAME
#define DEVICE_BLUETOOTH_NAME "Handy Mini"
#endif

#define DEVICE_MANUFACTURER_NAME "M5Stack"

volatile bool isBleConnected = false;
BLEHIDDevice *hid = nullptr;
BLECharacteristic *input;
BLECharacteristic *output;

extern Battery battery;

const uint16_t CCCD_NOTIFICATIONS_ENABLED = 0x0001;

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

  BLEAdvertising *advertising = server->getAdvertising();
  advertising->start();

  M5_LOGI("Client has disconnected, restarted advertising");
}

void OutputCallbacks::onWrite(BLECharacteristic *characteristic) {
  OutputReport *report = (OutputReport *)characteristic->getData();
}

class InputReportCCCDCallbacks : public BLEDescriptorCallbacks {
  void onWrite(BLEDescriptor *descriptor) override {
    uint16_t cccdValue = *(uint16_t *)descriptor->getValue();
    if (cccdValue & CCCD_NOTIFICATIONS_ENABLED) {
      if (xConnectionSemaphore != NULL) {
        xSemaphoreGive(xConnectionSemaphore);
      }
    }
  }
};

void batteryUpdateTask(void *) {
  M5_LOGI("Battery update task started");
  vTaskDelay(pdMS_TO_TICKS(2000));

  while (true) {
    if (hid != nullptr && isBleConnected) {
      uint8_t batteryLevel = battery.getBatteryLevel();
      hid->setBatteryLevel(batteryLevel);
      M5_LOGD("Battery level updated: %d%%", batteryLevel);
    }

    vTaskDelay(pdMS_TO_TICKS(240000));
  }
}

void bluetoothTask(void *) {
  BLEDevice::init(DEVICE_BLUETOOTH_NAME);
  BLEServer *server = BLEDevice::createServer();
  server->setCallbacks(new BleKeyboardCallbacks());

  hid = new BLEHIDDevice(server);
  input = hid->inputReport(1);
  output = hid->outputReport(1);

  BLE2902 *cccDesc =
      (BLE2902 *)input->getDescriptorByUUID(BLEUUID((uint16_t)0x2902));
  cccDesc->setCallbacks(new InputReportCCCDCallbacks());

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

  BLEDevice::setPower(ESP_PWR_LVL_N9, ESP_BLE_PWR_TYPE_ADV);
  BLEDevice::setPower(ESP_PWR_LVL_N9, ESP_BLE_PWR_TYPE_DEFAULT);
  advertising->setMinInterval(512);
  advertising->setMaxInterval(1024);

  advertising->start();

  M5_LOGI("Bluetooth task started. Waiting for connections.");

  xTaskCreate(batteryUpdateTask, "BatteryUpdate", 4096, NULL, 1, NULL);

  vTaskDelete(NULL);
}
