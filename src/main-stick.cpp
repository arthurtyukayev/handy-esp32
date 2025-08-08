#include "battery/battery.h"
#include "bluetooth/bluetooth.h"
#include "keyboard/keyboard.h"
#include "screen/screen.h"
#include "shared.h"

#include <Arduino.h>
#include <M5Unified.h>

#define BUTTON_HOLD_THRESHOLD 100

Battery battery;

TaskHandle_t xBTConnectingScreenTaskHandle = NULL;
TaskHandle_t xBTConnectedScreenTaskHandle = NULL;
SemaphoreHandle_t xConnectionSemaphore = NULL;

void connectionManagerTask(void *) {
  for (;;) {
    if (xSemaphoreTake(xConnectionSemaphore, portMAX_DELAY) == pdTRUE) {
      if (xBTConnectingScreenTaskHandle != NULL) {
        vTaskDelete(xBTConnectingScreenTaskHandle);
        xBTConnectingScreenTaskHandle = NULL;
      }

      if (xBTConnectedScreenTaskHandle == NULL) {
        xTaskCreate(showBTConnectedScreen, "connectedScreen", 4096, NULL, 6,
                    &xBTConnectedScreenTaskHandle);
      }

      while (isBleConnected) {
        vTaskDelay(pdMS_TO_TICKS(100));
      }

      if (xBTConnectedScreenTaskHandle != NULL) {
        vTaskDelete(xBTConnectedScreenTaskHandle);
        xBTConnectedScreenTaskHandle = NULL;
      }

      if (xBTConnectingScreenTaskHandle == NULL) {
        xTaskCreate(showBTConnectingScreen, "connectingScreen", 4096, NULL, 6,
                    &xBTConnectingScreenTaskHandle);
      }
    }
  }
}

void setup() {
  Serial.begin(115200);

  xConnectionSemaphore = xSemaphoreCreateBinary();

  xTaskCreate(bluetoothTask, "bluetooth", 8192, NULL, 5, NULL);
  xTaskCreate(connectionManagerTask, "connManager", 4096, NULL, 4, NULL);

  M5.begin();
  M5.BtnA.setHoldThresh(BUTTON_HOLD_THRESHOLD);

  battery.begin();

  xTaskCreate(showBTConnectingScreen, "connectingScreen", 4096, NULL, 6,
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

  M5.delay(1);
}
