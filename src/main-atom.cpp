#include "bluetooth/bluetooth.h"
#include "keyboard/keyboard.h"
#include "led/neopixel.h"

#include <Arduino.h>
#include <M5Unified.h>

#define BUTTON_HOLD_THRESHOLD 100

NeoPixel led;

TaskHandle_t xLedFlashTaskHandle = NULL;
bool wasShownConnectedLed = false;

void flashPixel(void *) {
  while (true) {
    led.setRed();
    delay(250);
    led.clear();
    delay(250);
  }
};

void setup() {
  Serial.begin(115200);

  xTaskCreate(bluetoothTask, "bluetooth", 20000, NULL, 5, NULL);

  led.begin();
  led.setRed();

  M5.begin();
  M5.BtnA.setHoldThresh(BUTTON_HOLD_THRESHOLD);

  xTaskCreate(flashPixel, "ledflash", 5000, NULL, 6, &xLedFlashTaskHandle);
}
void loop() {
  M5.update();
  M5.delay(1);

  if (isBleConnected) {
    if (xLedFlashTaskHandle != NULL) {
      vTaskDelete(xLedFlashTaskHandle);
      xLedFlashTaskHandle = NULL;
      led.clear();
    }

    if (!wasShownConnectedLed) {
      M5.delay(250);
      led.setGreen();
      M5.delay(1000);
      led.clear();
      wasShownConnectedLed = true;
    }

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
    M5.delay(1);
  } else if (xLedFlashTaskHandle == NULL) {
    wasShownConnectedLed = false;
    xTaskCreate(flashPixel, "ledflash", 5000, NULL, 6, &xLedFlashTaskHandle);
  }

  M5.delay(1);
}
