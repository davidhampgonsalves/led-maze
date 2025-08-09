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

int indexToPos(int xy) {
  return (xy * PX_SIZE) + PX_CENTER; // return center
}

int posToIndex(unsigned long xy) {
  return round(xy / PX_SIZE);
}

CRGB colorAt(int x, int y, CRGB level[]) {
  if(x < 0 || y < 0 || x >= MAX_X || y >= MAX_Y) return HIDDEN_WALL_COLOR;

  return level[x * MAX_Y + y];
}

void setShimmerLed(unsigned long elapsed, int x, int y, CHSV hsv, CRGB leds[]) {
  uint8_t shimmer_val = sin8(elapsed / 10 + x * 12 + y * 12);
  hsv.val = map(shimmer_val, 0, 255, 60, 100);
  setLed(x, y, hsv2rgb_spectrum(hsv), leds);
}

void setFlameLed(int x, int y, CRGB leds[]) {
  if(random(10) > 8) return;
  uint8_t r  = random(180, 255);
  uint8_t g  = random(0, 100);
  setLed(x, y, CRGB{ r, g, 0 }, leds);
}

void animateRing(unsigned long elapsed, CRGB c, State next, bool outward, int x, int y, CRGB leds[]) {
  setLed(x, y, BLACK, leds);

  if(elapsed > 1200) return updateState(next);

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

    setLed(x + 1, y, BLACK, leds);
    setLed(x - 1, y, BLACK, leds);
    setLed(x, y + 1, BLACK, leds);
    setLed(x, y - 1, BLACK, leds);
  } else if (elapsed >= 400 && elapsed < 700) {
    setLed(x + 1, y, c, leds);
    setLed(x - 1, y, c, leds);
    setLed(x, y + 1, c, leds);
    setLed(x, y - 1, c, leds);
  } else if (elapsed >= 800 && elapsed <= 1100) {
    setLed(x, y, c, leds);
  }
}