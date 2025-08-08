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
  int barWidth = M5.Display.width() * batteryLevel / 100;
  int barHeight = 10;
  int barY = M5.Display.height() - 10;

  uint16_t barColor;
  if (batteryLevel < 33) {
    barColor = RED;
  } else if (batteryLevel < 66) {
    barColor = YELLOW;
  } else {
    barColor = GREEN;
  }

  M5.Display.fillRect(0, barY, M5.Display.width(), barHeight, DARKGREY);
  M5.Display.fillRect(0, barY, barWidth, barHeight, barColor);

  M5.Display.setTextSize(1);
  M5.Display.setTextColor(BLACK);
  int textX = 2;
  int textY = barY + 1;
  M5.Display.setCursor(textX, textY);
  M5.Display.println("Battery");
}

void showBTConnectingScreen(void *) {
  M5.Display.setRotation(0);
  M5.Display.fillScreen(BLACK);
  M5.Display.setTextColor(WHITE);
  M5.Display.setTextSize(1);

  drawHeader(2, 20, 1);

  const int iconWidth = 64;
  const int iconHeight = 64;
  const int centerX = (M5.Display.width() - iconWidth * 2) / 2;
  const int iconY = 60;

  uint8_t batteryLevel = battery.getBatteryLevel();
  drawBatteryIndicator(batteryLevel);

  int dotCount = 0;
  while (true) {
    M5.Display.fillRect(30, M5.Display.height() - 60, M5.Display.width() - 30,
                        20, BLACK);

    M5.Display.setTextColor(WHITE);
    M5.Display.setTextSize(1);
    M5.Display.setCursor(2, M5.Display.height() / 2 - 10);
    M5.Display.print("Connecting");

    for (int i = 0; i < (dotCount % 4); i++) {
      M5.Display.print(".");
    }

    if (dotCount % 10 == 0) {
      batteryLevel = battery.getBatteryLevel();
      drawBatteryIndicator(batteryLevel);
    }

    dotCount++;
    vTaskDelay(pdMS_TO_TICKS(500));
  }
}

void showBTConnectedScreen(void *) {
  M5.Display.setBrightness(0);
  M5.Display.sleep();

  bool screenActive = false;

  static const size_t record_length = 256;
  static const size_t record_samplerate = 16000;
  static int16_t samples[record_length];

  const int meterTop = 18;
  const int meterBottom = M5.Display.height() - 12;
  const int meterHeight = meterBottom - meterTop;
  const int meterLeft = 0;
  const int meterWidth = M5.Display.width();

  float smoothedLevel = 0.0f;
  const float attack = 0.6f;
  const float release = 0.2f;

  int prevBarH = 0;
  uint32_t batteryTick = 0;

  while (true) {
    M5.update();

    if (M5.BtnA.isPressed() && !screenActive) {
      M5.Display.wakeup();
      M5.Display.setBrightness(100);
      M5.Display.setRotation(0);
      M5.Display.fillScreen(BLACK);

      if (M5.Speaker.isEnabled()) {
        M5.Speaker.end();
      }
      {
        auto cfg = M5.Mic.config();
        cfg.noise_filter_level = 0;
        M5.Mic.config(cfg);
      }
      M5.Mic.begin();

      drawHeader(2, 6, 1);

      uint8_t batteryLevel = battery.getBatteryLevel();
      drawBatteryIndicator(batteryLevel);

      M5.Display.fillRect(meterLeft, meterTop, meterWidth, meterHeight, BLACK);

      smoothedLevel = 0.0f;
      prevBarH = 0;
      batteryTick = 0;
      screenActive = true;
    }

    if (screenActive) {
      if ((batteryTick++ % 10) == 0) {
        uint8_t batteryLevel = battery.getBatteryLevel();
        drawBatteryIndicator(batteryLevel);
      }

      if (M5.Mic.isEnabled()) {
        if (M5.Mic.record(samples, record_length, record_samplerate)) {
          uint64_t sumsq = 0;
          for (size_t i = 0; i < record_length; ++i) {
            int32_t v = samples[i];
            sumsq += (uint64_t)(v * (int64_t)v);
          }
          float rms =
              sqrtf((float)sumsq / (float)record_length) / 32768.0f;

          float peak = 0.0f;
          for (size_t i = 0; i < record_length; ++i) {
            float a = fabsf(samples[i] / 32768.0f);
            if (a > peak)
              peak = a;
          }
          float level = (0.6f * rms + 0.4f * peak) * 5.0f;

          float alpha = (level > smoothedLevel) ? attack : release;
          smoothedLevel = smoothedLevel + alpha * (level - smoothedLevel);
          if (smoothedLevel < 0.0f)
            smoothedLevel = 0.0f;

          int barH = (int)(smoothedLevel * meterHeight);
          if (barH < 0)
            barH = 0;
          if (barH > meterHeight)
            barH = meterHeight;

          uint16_t color = DARKGREY;

          if (barH > prevBarH) {
            int dh = barH - prevBarH;
            M5.Display.fillRect(meterLeft, meterBottom - barH, meterWidth, dh,
                                color);
          } else if (barH < prevBarH) {
            int dh = prevBarH - barH;
            M5.Display.fillRect(meterLeft, meterBottom - prevBarH, meterWidth,
                                dh, BLACK);
          }
          prevBarH = barH;
        }
      }
    }

    if (!M5.BtnA.isPressed() && screenActive) {
      if (M5.Mic.isEnabled()) {
        M5.Mic.end();
      }

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

  uint8_t batteryLevel = battery.getBatteryLevel();
  drawBatteryIndicator(batteryLevel);
}
