#include <FastLED.h>
#include "common.h"

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
  return xy * 1000;
}

CRGB colorAt(int x, int y, CRGB leds[]) {
  return leds[x * MAX_Y + y];
}