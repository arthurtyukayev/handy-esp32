#include "atom-base.h"
#include <Arduino.h>
#include <M5Unified.h>

#define ATOM_BAT_ADC_PIN 33 // For Atom Series
#define BAT_ADC_RESOLUTION 12

void AtomBaseBattery::begin() {
  pinMode(ATOM_BAT_ADC_PIN, INPUT);
  analogReadResolution(BAT_ADC_RESOLUTION);
}

uint32_t AtomBaseBattery::getAdcVoltage() {
  uint32_t adc_vol = 0;
  adc_vol = analogReadMilliVolts(ATOM_BAT_ADC_PIN);
  return adc_vol;
}

uint32_t AtomBaseBattery::getBatteryVoltage() {
  uint32_t adc_vol = getAdcVoltage();
  uint32_t bat_vol = adc_vol * 2;
  return bat_vol;
}

uint8_t AtomBaseBattery::getBatteryLevel() {
  uint32_t voltage = getBatteryVoltage();
  float voltageInVolts = voltage / 1000.0f;

  if (voltageInVolts <= 3.00f) {
    return 0;
  } else if (voltageInVolts <= 3.47f) {
    return (uint8_t)((voltageInVolts - 3.00f) / 0.47f * 25.0f);
  } else if (voltageInVolts <= 3.61f) {
    return (uint8_t)(25.0f + (voltageInVolts - 3.48f) / 0.13f * 25.0f);
  } else if (voltageInVolts <= 3.81f) {
    return (uint8_t)(50.0f + (voltageInVolts - 3.62f) / 0.19f * 25.0f);
  } else if (voltageInVolts <= 4.20f) {
    return (uint8_t)(75.0f + (voltageInVolts - 3.82f) / 0.38f * 25.0f);
  } else {
    return 100;
  }
}
