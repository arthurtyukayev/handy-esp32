#ifndef SCREEN_H
#define SCREEN_H

#include <M5Unified.h>

void drawBatteryIndicator(uint8_t batteryLevel);
void showBTConnectingScreen(void *);
void showBTConnectedScreen(void *);
void showBTDisconnectedScreen();

#endif
