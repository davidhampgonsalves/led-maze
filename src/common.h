#pragma once

#include <FastLED.h>

const int MAX_X = 6;
const int MAX_Y = 50;
const int MAX_PX = MAX_X * MAX_Y;

void setLed(int x, int y, CHSV c, CRGB leds[]);
void setLed(int x, int y, CRGB c, CRGB leds[]);
int indexToPos(int xy);
CRGB colorAt(int x, int y, CRGB leds[]);