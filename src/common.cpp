#include <Arduino.h>
#include <FastLED.h>
#include "pixeltypes.h"
#include <cmath>
#include "common.h"
#include "state.h"
#include "colors.h"

void setLed(int x, int y, CRGB c, CRGB leds[]) {
  if(x < 0 || y < 0 || x >= MAX_X || y >= MAX_Y) return;

  if(x % 2 == 1) y = MAX_Y - y - 1;

  leds[x * MAX_Y + y] = c;
}

// void setLed(int x, int y, CHSV c, CRGB leds[]) {
//   if(x % 2 == 1) y = MAX_Y - y - 1;

//   c.value = dim8_lin(c.value); // gamma correction

//   leds[x * MAX_Y + y] = c;
// }

int indexToPos(int xy) {
  return (xy * PX_SIZE) + PX_CENTER; // return center
}

int posToIndex(unsigned long xy) {
  return round(xy / PX_SIZE);
}

CRGB colorAt(int x, int y, CRGB leds[]) {
  if(x < 0 || y < 0 || x >= MAX_X || y >= MAX_Y) return HIDDEN_WALL_COLOR;

  return leds[x * MAX_Y + y];
}

const uint8_t SHIMMER_BRIGHTNESS_VARIATION = 30; // Max brightness variation (0-255)
const uint8_t SHIMMER_HUE_VARIATION = 5;      // Max hue variation (0-255)
const uint8_t SHIMMER_SATURATION_VARIATION = 20; // Max saturation variation (0-255)
const uint16_t SHIMMER_SPEED = 20;             // Controls how fast the shimmer changes
void shimmer(unsigned long elapsed, int x, int y, CHSV hsv, CRGB leds[]) {
    uint8_t brightnessOffset = sin8_C(elapsed / SHIMMER_SPEED + 0) * SHIMMER_BRIGHTNESS_VARIATION / 255;
    uint8_t hueOffset = sin8_C(elapsed / (SHIMMER_SPEED * 1.5) + 100) * SHIMMER_HUE_VARIATION / 255;
    uint8_t saturationOffset = sin8_C(elapsed / (SHIMMER_SPEED * 0.7) + 200) * SHIMMER_SATURATION_VARIATION / 255;

    hsv.val = constrain(hsv.val + brightnessOffset - (SHIMMER_BRIGHTNESS_VARIATION / 2), 0, 255);
    hsv.hue += hueOffset - (SHIMMER_HUE_VARIATION / 2);
    hsv.sat = constrain(hsv.sat + saturationOffset - (SHIMMER_SATURATION_VARIATION / 2), 0, 255);

    setLed(x, y, hsv2rgb_spectrum(hsv), leds);
}

void setFlameLed(int x, int y, CRGB leds[]) {
  if(random(10) > 8) return;
  uint8_t r  = random(180, 255);
  uint8_t g  = random(0, 100);
  setLed(x, y, CRGB{ r, g, 0 }, leds);
}

void setPortalLed(unsigned long elapsed, int x, int y, CRGB leds[]) {
  shimmer(elapsed, x, y, CHSV{ 202, 100, 100 }, leds);
}

void animateRing(unsigned long elapsed, CRGB c, State next, bool outward, int x, int y, CRGB leds[]) {
  setLed(x, y, BLACK, leds);

  if(outward)
    elapsed = 1200 - elapsed;

  if (elapsed >= 0 && elapsed < 300) {
    setLed(x + 2, y, c, leds);
    setLed(x - 2, y, c, leds);
    setLed(x, y + 2, c, leds);
    setLed(x, y - 2, c, leds);

    setLed(x + 1, y + 1, c, leds);
    setLed(x + 1, y - 1, c, leds);
    setLed(x - 1, y + 1, c, leds);
    setLed(x - 1, y - 1, c, leds);
  } else if (elapsed >= 400 && elapsed < 700) {
    setLed(x + 1, y, c, leds);
    setLed(x - 1, y, c, leds);
    setLed(x, y + 1, c, leds);
    setLed(x, y - 1, c, leds);
  } else if (elapsed >= 800 && elapsed <= 1100) {
    setLed(x, y, c, leds);
  } else
    updateState(next);
}