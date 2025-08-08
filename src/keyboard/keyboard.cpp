#include "keyboard.h"

const InputReport NO_KEY_PRESSED = {};

void sendKey() {
  constexpr uint8_t HID_KEY_I = 0x0C;
  constexpr uint8_t RIGHT_ALT_KEY = 0x40;

  InputReport report = {
      .modifiers = RIGHT_ALT_KEY,
      .reserved = 0,
      .pressedKeys = {HID_KEY_I, 0, 0, 0, 0, 0},
  };

  input->setValue((uint8_t *)&report, sizeof(report));
  input->notify();
}

void clearKey() {
  input->setValue((uint8_t *)&NO_KEY_PRESSED, sizeof(NO_KEY_PRESSED));
  input->notify();
}
