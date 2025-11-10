
#pragma once

#include <FastLED.h>

void racingAnimationInit(CRGB* leds);
void racingAnimation(unsigned long elapsed, CRGB* leds);
void rainbowAnimationInit();
void rainbowAnimation(unsigned long elapsed, CRGB* leds);