#pragma once

#include <cstdint>
class AtomBaseBattery {
public:
  void begin();
  uint32_t getAdcVoltage();
  uint32_t getBatteryVoltage();
  uint8_t getBatteryLevel();
};
