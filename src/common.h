#pragma once

#include <FastLED.h>
#include "state.h"

const int MAX_X = 6;
const int MAX_Y = 50;
const int MAX_PX = MAX_X * MAX_Y;

struct Pt {
  int x;
  int y;
};

void setLed(int x, int y, CHSV c, CRGB leds[]);
void setLed(int x, int y, CRGB c, CRGB leds[]);
int indexToPos(int xy);
int posToIndex(unsigned long xy);
CRGB colorAt(int x, int y, CRGB leds[]);
void animateRing(unsigned long elapsed, CRGB c, State next, bool outward, int x, int y, CRGB leds[]);