#include <Arduino.h>
#include <FastLED.h>
#include <cmath>
#include "common.h"
#include "state.h"
#include "colors.h"

void setLed(int x, int y, CRGB c, CRGB leds[]) {
  if(x < 0 || y < 0 || x >= MAX_X || y >= MAX_Y) return;

  if(x % 2 == 1) y = MAX_Y - y - 1;

  // c.value = dim8_lin(c.value); // gamma correction

  leds[x * MAX_Y + y] = c;
}

void setLed(int x, int y, CHSV c, CRGB leds[]) {
  if(x % 2 == 1) y = MAX_Y - y - 1;

  c.value = dim8_lin(c.value); // gamma correction

  leds[x * MAX_Y + y] = c;
}

int indexToPos(int xy) {
  return (xy * 1000) + 500; // return center
}

int posToIndex(unsigned long xy) {
  return round(xy / 1000);
}

CRGB colorAt(int x, int y, CRGB leds[]) {
  return leds[x * MAX_Y + y];
}

void animateRing(unsigned long elapsed, CRGB c, State next, bool outward, int x, int y, CRGB leds[]) {
  setLed(x, y, BLACK, leds);

  if(outward)
    elapsed = 1200 - elapsed;

  if (elapsed >= 0 && elapsed < 400) {
    setLed(x + 2, y, c, leds);
    setLed(x - 2, y, c, leds);
    setLed(x, y + 2, c, leds);
    setLed(x, y - 2, c, leds);

    setLed(x + 1, y + 1, c, leds);
    setLed(x + 1, y - 1, c, leds);
    setLed(x - 1, y + 1, c, leds);
    setLed(x - 1, y - 1, c, leds);
  } else if (elapsed >= 400 && elapsed < 800) {
    setLed(x + 1, y, c, leds);
    setLed(x - 1, y, c, leds);
    setLed(x, y + 1, c, leds);
    setLed(x, y - 1, c, leds);
  } else if (elapsed >= 800 && elapsed <= 1200) {
    setLed(x, y, c, leds);
  } else
    updateState(next);
}