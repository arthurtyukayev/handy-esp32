#include "neopixel.h"
#include <Adafruit_NeoPixel.h>
#include <sys/types.h>

#define LED_PIN 27
#define LED_COUNT 1

Adafruit_NeoPixel pixels =
    Adafruit_NeoPixel(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

uint32_t red = pixels.Color(255, 0, 0);
uint32_t white = pixels.Color(255, 255, 255);
uint32_t green = pixels.Color(0, 255, 0);
uint32_t black = pixels.Color(0, 0, 0);

void NeoPixel::begin() {
  pixels.begin();
  setRed();
}

void NeoPixel::setRed() {
  pixels.setPixelColor(0, red);
  pixels.show();
}
void NeoPixel::setGreen() {
  pixels.setPixelColor(0, green);
  pixels.show();
}
void NeoPixel::setWhite() {
  pixels.setPixelColor(0, white);
  pixels.show();
}

void NeoPixel::clear() {
  pixels.setPixelColor(0, black);
  pixels.show();
}
