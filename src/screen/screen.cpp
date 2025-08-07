#include "screen.h"
#include "../battery/battery.h"
#include <M5Unified.h>

extern Battery battery;

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
  // Initial state - screen OFF to save battery
  M5.Display.setBrightness(0);
  M5.Display.sleep();
  
  bool screenActive = false;
  
  while (true) {
    // Check for button press - turn screen ON only when button is pressed
    if (M5.BtnA.isPressed() && !screenActive) {
      // Wake screen and set up the sleek UI
      M5.Display.wakeup();
      M5.Display.setBrightness(100);
      M5.Display.setRotation(0);
      M5.Display.fillScreen(BLACK);
     
      uint8_t batteryLevel = battery.getBatteryLevel();
      // Display "TALKING" text
      M5.Display.setTextSize(2);
      M5.Display.setCursor(8, M5.Display.height()/2 - 10);
      M5.Display.println("TALKING");
      
      // Display battery text
      M5.Display.setTextSize(1);
      M5.Display.setCursor(8, M5.Display.height() - 40);
      M5.Display.println("Battery");
      
      // Draw battery level bar
      int barWidth = M5.Display.width() * batteryLevel / 100;
      int barHeight = 10;
      int barY = M5.Display.height() - 10;
      
      // Choose color based on battery level
      uint16_t barColor;
      if (batteryLevel < 33) {
        barColor = RED;
      } else if (batteryLevel < 66) {
        barColor = YELLOW;
      } else {
        barColor = GREEN;
      }
      
      // Draw background and foreground bars
      M5.Display.fillRect(0, barY, M5.Display.width(), barHeight, DARKGREY);
      M5.Display.fillRect(0, barY, barWidth, barHeight, barColor);
      
      screenActive = true;
    }
    
    if (!M5.BtnA.isPressed() && screenActive) {
      M5.Display.setBrightness(0);
      M5.Display.sleep();
      screenActive = false;
    }
    
    M5.update();
    vTaskDelay(pdMS_TO_TICKS(50));
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
