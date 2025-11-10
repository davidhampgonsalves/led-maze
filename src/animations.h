
#pragma once

#include <FastLED.h>

unsigned long nextAnimationFrameAt = 0;
unsigned int frameCount;
unsigned int ballX;
unsigned int ballY;
bool ballDirRight = true;

void racingAnimationInit(CRGB* leds);
void racingAnimation(unsigned long elapsed, CRGB* leds);