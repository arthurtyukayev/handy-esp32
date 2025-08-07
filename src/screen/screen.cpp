#include "screen.h"
#include <M5Unified.h>

void showBTConnectingScreen(void *) {
  M5.Display.setRotation(0);
  M5.Display.fillScreen(BLACK);
  M5.Display.setTextColor(WHITE);
  M5.Display.setTextSize(1);

  // Draw static elements once
  // Title
  M5.Display.setCursor(10, 20);
  M5.Display.setTextSize(1);
  M5.Display.println("Handy ESP32");

  // Draw bitmap icon
  const int iconWidth = 64;  // Width of the bitmap array
  const int iconHeight = 64; // Height of the bitmap array
  const int centerX = (M5.Display.width() - iconWidth * 2) / 2;
  const int iconY = 60;

  int dotCount = 0;
  while (true) {
    // Clear only the dots area
    M5.Display.fillRect(30, M5.Display.height() - 30, M5.Display.width() - 30,
                        20, BLACK);

    // Connection status
    M5.Display.setCursor(30, M5.Display.height() - 30);
    M5.Display.setTextSize(1);
    M5.Display.print("Connecting");

    // Animated dots
    for (int i = 0; i < (dotCount % 4); i++) {
      M5.Display.print(".");
    }

    dotCount++;
    vTaskDelay(pdMS_TO_TICKS(500));
  }
}

void showBTConnectedScreen(void *) {
  M5.Display.setRotation(0);
  M5.Display.fillScreen(GREEN);
  M5.Display.setTextColor(BLACK);
  M5.Display.setTextSize(2);

  // Title
  M5.Display.setCursor(10, 20);
  M5.Display.println("Handy");

  // Status
  M5.Display.setCursor(10, 50);
  M5.Display.println("Connected!");

  // Instructions
  M5.Display.setTextSize(1);
  M5.Display.setCursor(10, 80);
  M5.Display.println("Hold A to talk");
  M5.Display.setCursor(10, 95);
  M5.Display.println("Release to stop");

  while (true) {
    vTaskDelay(pdMS_TO_TICKS(100));
  }
}

void showBTDisconnectedScreen() {
  M5.Display.setRotation(0);
  M5.Display.fillScreen(RED);
  M5.Display.setTextColor(WHITE);
  M5.Display.setTextSize(2);

  M5.Display.setCursor(10, 20);
  M5.Display.println("Handy");

  M5.Display.setCursor(10, 50);
  M5.Display.println("Disconnected");

  M5.Display.setTextSize(1);
  M5.Display.setCursor(10, 80);
  M5.Display.println("Reset to reconnect");
}
