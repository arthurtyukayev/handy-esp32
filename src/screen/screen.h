#ifndef SCREEN_H
#define SCREEN_H

#include <M5Unified.h>

void drawHeader(int x, int y, int textSize);
void drawBatteryIndicator(uint8_t batteryLevel);
void showBTConnectingScreen(void *);
void showBTConnectedScreen(void *);
void showBTDisconnectedScreen();

#endif
