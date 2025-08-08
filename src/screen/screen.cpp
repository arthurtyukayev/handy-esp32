#include "screen.h"
#include "../battery/battery.h"
#include "../shared.h"
#include <M5Unified.h>

extern Battery battery;

void drawHeader(int x, int y, int textSize) {
  M5.Display.setTextColor(WHITE);
  M5.Display.setCursor(x, y);
  M5.Display.setTextSize(textSize);
  M5.Display.println(DEVICE_BLUETOOTH_NAME);
}

void drawBatteryIndicator(uint8_t batteryLevel) {
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

  // Display battery text on top of the battery bar
  M5.Display.setTextSize(1);
  M5.Display.setTextColor(BLACK);
  // Center the text directly on top of the battery bar
  int textX = 2;
  int textY =
      barY + 1; // Position text on top of the battery bar with a small offset
  M5.Display.setCursor(textX, textY);
  M5.Display.println("Battery");
}

void showBTConnectingScreen(void *) {
  M5.Display.setRotation(0);
  M5.Display.fillScreen(BLACK);
  M5.Display.setTextColor(WHITE);
  M5.Display.setTextSize(1);

  // Draw static elements once
  // Title with device name from build flags
  drawHeader(2, 20, 1);

  // Draw bitmap icon
  const int iconWidth = 64;  // Width of the bitmap array
  const int iconHeight = 64; // Height of the bitmap array
  const int centerX = (M5.Display.width() - iconWidth * 2) / 2;
  const int iconY = 60;
  
  // Initial battery display
  uint8_t batteryLevel = battery.getBatteryLevel();
  drawBatteryIndicator(batteryLevel);

  int dotCount = 0;
  while (true) {
    // Clear only the dots area
    M5.Display.fillRect(30, M5.Display.height() - 60, M5.Display.width() - 30,
                        20, BLACK);

    // Connection status
    M5.Display.setTextColor(WHITE);
    M5.Display.setTextSize(1);
    M5.Display.setCursor(2, M5.Display.height() / 2 - 10);
    M5.Display.print("Connecting");

    // Animated dots
    for (int i = 0; i < (dotCount % 4); i++) {
      M5.Display.print(".");
    }

    // Update battery level every 5 seconds (10 iterations)
    if (dotCount % 10 == 0) {
      batteryLevel = battery.getBatteryLevel();
      drawBatteryIndicator(batteryLevel);
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
 
  // Audio capture config
  static const size_t record_length = 256;        // samples per frame
  static const size_t record_samplerate = 16000;  // Hz
  static int16_t samples[record_length];
 
  // Volume meter layout
  const int meterTop = 18;                         // below header
  const int meterBottom = M5.Display.height() - 12; // above battery bar
  const int meterHeight = meterBottom - meterTop;
  const int meterLeft = 0;
  const int meterWidth = M5.Display.width();
 
  // Smoothing for volume level (EMA)
  float smoothedLevel = 0.0f;   // unbounded (we'll clamp only when drawing)
  const float attack = 0.6f;    // faster rise
  const float release = 0.2f;   // slower fall

  int prevBarH = 0;             // to minimize redraw (avoid flicker)
  uint32_t batteryTick = 0;
 
  while (true) {
    // Update input state first
    M5.update();
 
    // Check for button press - turn screen ON only when button is pressed
    if (M5.BtnA.isPressed() && !screenActive) {
      // Wake screen and set up the sleek UI
      M5.Display.wakeup();
      M5.Display.setBrightness(100);
      M5.Display.setRotation(0);
      M5.Display.fillScreen(BLACK);
 
      // Microphone requires speaker to be off on many devices
      if (M5.Speaker.isEnabled()) { M5.Speaker.end(); }
      // Set mic to minimal noise filtering for higher sensitivity
      {
        auto cfg = M5.Mic.config();
        cfg.noise_filter_level = 0;
        M5.Mic.config(cfg);
      }
      M5.Mic.begin();
 
      // Display header
      drawHeader(2, 6, 1);
 
      // Draw initial battery indicator
      uint8_t batteryLevel = battery.getBatteryLevel();
      drawBatteryIndicator(batteryLevel);
 
      // Clear meter area
      M5.Display.fillRect(meterLeft, meterTop, meterWidth, meterHeight, BLACK);

      smoothedLevel = 0.0f;
      prevBarH = 0;
      batteryTick = 0;
      screenActive = true;
    }
 
    // While active, capture mic and draw volume meter
    if (screenActive) {
      // Periodically refresh battery indicator (about every 500ms)
      if ((batteryTick++ % 10) == 0) {
        uint8_t batteryLevel = battery.getBatteryLevel();
        drawBatteryIndicator(batteryLevel);
      }
 
      if (M5.Mic.isEnabled()) {
        if (M5.Mic.record(samples, record_length, record_samplerate)) {
          // Compute normalized RMS (0..1) from int16 samples
          // Note: int16 full-scale is 32767; we scale accordingly.
          uint64_t sumsq = 0;
          for (size_t i = 0; i < record_length; ++i) {
            int32_t v = samples[i];
            sumsq += (uint64_t)(v * (int64_t)v);
          }
          float rms = sqrtf((float)sumsq / (float)record_length) / 32768.0f; // 0..~1
 
          // Remove clamp/noise gate; increase sensitivity and responsiveness
          // Compute both RMS and Peak, combine for a snappy yet stable meter
          float peak = 0.0f;
          for (size_t i = 0; i < record_length; ++i) {
            float a = fabsf(samples[i] / 32768.0f);
            if (a > peak) peak = a;
          }
          // Increase sensitivity with gain; mix peak and rms (weighted)
          float level = (0.6f * rms + 0.4f * peak) * 5.0f;  // 5x gain

          // No clamping here; only clamp for drawing within screen bounds

          // Exponential smoothing with separate attack/release
          float alpha = (level > smoothedLevel) ? attack : release;
          smoothedLevel = smoothedLevel + alpha * (level - smoothedLevel);
          if (smoothedLevel < 0.0f) smoothedLevel = 0.0f; // keep non-negative

          // Map to meter height; clamp only for drawing bounds
          int barH = (int)(smoothedLevel * meterHeight);
          if (barH < 0) barH = 0;
          if (barH > meterHeight) barH = meterHeight;

          // Fixed grey color for the meter bar
          uint16_t color = DARKGREY;

          // Minimize flicker: only draw the changed portion instead of clearing all
          if (barH > prevBarH) {
            // grew: draw the difference
            int dh = barH - prevBarH;
            M5.Display.fillRect(meterLeft, meterBottom - barH, meterWidth, dh, color);
          } else if (barH < prevBarH) {
            // shrank: erase the difference
            int dh = prevBarH - barH;
            M5.Display.fillRect(meterLeft, meterBottom - prevBarH, meterWidth, dh, BLACK);
          }
          prevBarH = barH;
        }
      }
    }
 
    // If button released, turn off and sleep
    if (!M5.BtnA.isPressed() && screenActive) {
      // Stop microphone to save power
      if (M5.Mic.isEnabled()) { M5.Mic.end(); }
 
      M5.Display.setBrightness(0);
      M5.Display.sleep();
      screenActive = false;
    }
 
    vTaskDelay(pdMS_TO_TICKS(50));
  }
}
void showBTDisconnectedScreen() {
  M5.Display.setRotation(0);
  M5.Display.fillScreen(RED);

  drawHeader(10, 20, 2);

  M5.Display.setCursor(10, 50);
  M5.Display.setTextSize(2);
  M5.Display.println("Disconnected");

  M5.Display.setTextSize(1);
  M5.Display.setCursor(10, 80);
  M5.Display.println("Reset to reconnect");
  
  // Add battery indicator to disconnected screen
  uint8_t batteryLevel = battery.getBatteryLevel();
  drawBatteryIndicator(batteryLevel);
}
