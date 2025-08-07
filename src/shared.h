#pragma once

#include <Arduino.h>

extern SemaphoreHandle_t xConnectionSemaphore;

// Make sure device name is accessible from platformio.ini build flags
#ifndef DEVICE_BLUETOOTH_NAME
#define DEVICE_BLUETOOTH_NAME "Handy ESP32"
#endif
