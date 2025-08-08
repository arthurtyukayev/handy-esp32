#pragma once

#include <Arduino.h>

extern SemaphoreHandle_t xConnectionSemaphore;

#ifndef DEVICE_BLUETOOTH_NAME
#define DEVICE_BLUETOOTH_NAME "Handy ESP32"
#endif
