#include "bluetooth/bluetooth.h"
#include "keyboard/keyboard.h"
#include <Arduino.h>
#include <M5Unified.h>

#define BUTTON_HOLD_THRESHOLD 100

void setup() {
  Serial.begin(115200);
  xTaskCreate(bluetoothTask, "bluetooth", 20000, NULL, 5, NULL);
  M5.begin();

  M5.BtnA.setHoldThresh(BUTTON_HOLD_THRESHOLD);
}

void loop() {
  M5.delay(1);
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
    } else if (talkButtonState == 4) {
      clearKey();
    }
  }
}
