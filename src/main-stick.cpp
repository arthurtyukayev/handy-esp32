#include "bluetooth/bluetooth.h"
#include "keyboard/keyboard.h"
#include "screen/screen.h"
#include "shared.h"

#include <Arduino.h>
#include <M5Unified.h>

#define BUTTON_HOLD_THRESHOLD 100

TaskHandle_t xBTConnectingScreenTaskHandle = NULL;
TaskHandle_t xBTConnectedScreenTaskHandle = NULL;
SemaphoreHandle_t xConnectionSemaphore = NULL;

void connectionManagerTask(void *) {
  if (xSemaphoreTake(xConnectionSemaphore, portMAX_DELAY) == pdTRUE) {
    if (xBTConnectingScreenTaskHandle != NULL) {
      vTaskDelete(xBTConnectingScreenTaskHandle);
      xBTConnectingScreenTaskHandle = NULL;
    }

    xTaskCreate(showBTConnectedScreen, "connectedScreen", 5000, NULL, 6,
                &xBTConnectedScreenTaskHandle);
  }

  vTaskDelete(NULL);
}

void setup() {
  Serial.begin(115200);

  xConnectionSemaphore = xSemaphoreCreateBinary();

  xTaskCreate(bluetoothTask, "bluetooth", 20000, NULL, 5, NULL);
  xTaskCreate(connectionManagerTask, "connManager", 5000, NULL, 4, NULL);

  M5.begin();
  M5.BtnA.setHoldThresh(BUTTON_HOLD_THRESHOLD);

  xTaskCreate(showBTConnectingScreen, "connectingScreen", 20000, NULL, 6,
              &xBTConnectingScreenTaskHandle);
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
    } else if (talkButtonState == 4) {
      clearKey();
    }
  }

  // M5.delay(1);
}
