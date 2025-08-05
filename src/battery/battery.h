#pragma once

#include <cstdint>
class Battery {
public:
  void begin();
  uint32_t getAdcVoltage();
  uint32_t getBatteryVoltage();
  uint8_t getBatteryLevel();
};
