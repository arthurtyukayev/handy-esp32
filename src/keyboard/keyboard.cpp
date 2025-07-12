#include "keyboard.h"
#include "HIDKeyboardTypes.h"
#include "HIDTypes.h"

const InputReport NO_KEY_PRESSED = {};

const uint8_t REPORT_MAP[] = {
    USAGE_PAGE(1),      0x01, USAGE(1),           0x06,
    COLLECTION(1),      0x01, REPORT_ID(1),       0x01,
    USAGE_PAGE(1),      0x07, USAGE_MINIMUM(1),   0xE0,
    USAGE_MAXIMUM(1),   0xE7, LOGICAL_MINIMUM(1), 0x00,
    LOGICAL_MAXIMUM(1), 0x01, REPORT_COUNT(1),    0x08,
    REPORT_SIZE(1),     0x01, HIDINPUT(1),        0x02,
    REPORT_COUNT(1),    0x01, REPORT_SIZE(1),     0x08,
    HIDINPUT(1),        0x01, REPORT_COUNT(1),    0x06,
    REPORT_SIZE(1),     0x08, LOGICAL_MINIMUM(1), 0x00,
    LOGICAL_MAXIMUM(1), 0x65, USAGE_MINIMUM(1),   0x00,
    USAGE_MAXIMUM(1),   0x65, HIDINPUT(1),        0x00,
    REPORT_COUNT(1),    0x05, REPORT_SIZE(1),     0x01,
    USAGE_PAGE(1),      0x08, USAGE_MINIMUM(1),   0x01,
    USAGE_MAXIMUM(1),   0x05, LOGICAL_MINIMUM(1), 0x00,
    LOGICAL_MAXIMUM(1), 0x01, HIDOUTPUT(1),       0x02,
    REPORT_COUNT(1),    0x01, REPORT_SIZE(1),     0x03,
    HIDOUTPUT(1),       0x01, END_COLLECTION(0)};

void sendKey() {
  const char *i = "i";
  const uint RIGHT_ALT_KEY = 64;
  KEYMAP map = keymap[(uint8_t)*i];

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
