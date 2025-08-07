#include "keyboard.h"
#include "HIDKeyboardTypes.h"

const InputReport NO_KEY_PRESSED = {};

void sendKey() {
  // Chnage the letter, the default is option + i / alt + i
  const char *i = "i";
  const uint RIGHT_ALT_KEY = 64;
  KEYMAP map = keymap[(uint8_t)*i];

  // Remove or change the modifier, default is RIGHT_ALT_KEY / Right option key 
  InputReport report = {.modifiers = RIGHT_ALT_KEY,
                        .reserved = 0,
                        .pressedKeys = {map.usage, 0, 0, 0, 0, 0}};

  input->setValue((uint8_t *)&report, sizeof(report));
  input->notify();
}

void clearKey() {
  input->setValue((uint8_t *)&NO_KEY_PRESSED, sizeof(NO_KEY_PRESSED));
  input->notify();
}
