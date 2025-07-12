#pragma once

#include "BLEHIDDevice.h"
#include <Arduino.h>

struct InputReport {
  uint8_t modifiers;
  uint8_t reserved;
  uint8_t pressedKeys[6];
};

struct OutputReport {
  uint8_t leds;
};

extern const InputReport NO_KEY_PRESSED;

void sendKey();
void clearKey();
void typeText(const char *text);

extern BLECharacteristic *input;
extern BLECharacteristic *output;
