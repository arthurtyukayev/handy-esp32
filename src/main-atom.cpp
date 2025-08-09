#include "battery/battery.h"
#include "bluetooth/bluetooth.h"
#include "keyboard/keyboard.h"
#include "led/neopixel.h"
#include "shared.h"
#include "utility/Log_Class.hpp"

#include <Arduino.h>
#include <M5Unified.h>

#define BUTTON_HOLD_THRESHOLD 100
NeoPixel led;
Battery battery;

TaskHandle_t xLedFlashTaskHandle = NULL;
SemaphoreHandle_t xConnectionSemaphore = NULL;

void flashPixel(void *) {
  while (true) {
    led.setRed();
    vTaskDelay(pdMS_TO_TICKS(250));
    led.clear();
    vTaskDelay(pdMS_TO_TICKS(250));
  }
};

void connectionManagerTask(void *) {
  while (true) {
    if (xSemaphoreTake(xConnectionSemaphore, portMAX_DELAY) == pdTRUE) {
      if (xLedFlashTaskHandle != NULL) {
        vTaskDelete(xLedFlashTaskHandle);
        xLedFlashTaskHandle = NULL;
        led.clear();
      }

      led.setGreen();
      vTaskDelay(pdMS_TO_TICKS(1000));
      led.clear();

      while (isBleConnected) {
        vTaskDelay(pdMS_TO_TICKS(100));
      }

      led.setRed();
    }
  }
}

void setup() {
  Serial.begin(115200);

  xConnectionSemaphore = xSemaphoreCreateBinary();

  xTaskCreate(bluetoothTask, "bluetooth", 20000, NULL, 5, NULL);
  xTaskCreate(connectionManagerTask, "connManager", 5000, NULL, 4, NULL);

  led.begin();
  battery.begin();

  auto cfg = M5.config();
  cfg.output_power = false;
  cfg.internal_mic = false;
  cfg.internal_spk = false;
  cfg.internal_imu = false;
  cfg.led_brightness = 0;
  M5.begin(cfg);
  M5.BtnA.setHoldThresh(BUTTON_HOLD_THRESHOLD);

  xTaskCreate(flashPixel, "ledflash", 5000, NULL, 6, &xLedFlashTaskHandle);
}

void loop() {
  M5.update();

  if (isBleConnected) {
    int talkButtonState = M5.BtnA.wasHold()               ? 1
                          : M5.BtnA.wasClicked()          ? 2
                          : M5.BtnA.wasPressed()          ? 3
                          : M5.BtnA.wasReleased()         ? 4
                          : M5.BtnA.wasDecideClickCount() ? 5
                                                          : 0;
    if (talkButtonState == 1) {
      sendKey();
      led.setWhite();
    } else if (talkButtonState == 4) {
      clearKey();
      led.clear();
    }
  }

  M5.delay(1);
}
