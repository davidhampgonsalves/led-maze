#pragma once

#include <FastLED.h>

const int WORD_WAIT = 1100;
const int WORD_WAIT_LONG = 2000;

void write(const char * text, CRGB leds[]);
void write(std::string text, CRGB leds[]);
void write(long num, CRGB leds[]);
void writeTitle(const char * text, CRGB leds[], unsigned long);