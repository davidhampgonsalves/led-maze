#pragma once

#include <FastLED.h>

const CRGB BLACK{ 0, 0, 0 };
const CRGB START_BURST{ 20, 255, 20 };
const CRGB DEATH_BURST{ 0, 238, 255 };
const CRGB TEXT_COLOR{ 150, 30, 0 };
const CRGB BALL_COLOR{ 0, 30, 200 };
const CRGB HIDDEN_WALL_COLOR{ 100, 100, 100 };

const CRGB EMPTY_RGB = CRGB(255, 255, 255);
const CRGB FIRE_RGB = CRGB(255, 0, 0);
const CRGB START_RGB = CRGB(255, 0, 255);
const CRGB FINISH_RGB = CRGB(0, 255, 0);
const CRGB PORTAL_RGB = CRGB(0, 101, 255);
const CRGB BREAKABLE_WALL_RGB = CRGB(150, 150, 150);
const CRGB SLOW_RGB = CRGB(120, 170, 110);

const CRGB H_FIRE_RGB = CRGB(1, 0, 0);
const CRGB H_WALL_RGB = CRGB(1, 1, 1);
const CRGB H_FINISH_RGB = CRGB(0, 1, 0);
const CRGB H_PORTAL_RGB = CRGB(0, 0, 1);

const CHSV SLOW_HSV = CHSV(110, 85, 85); // used by shimmer
const CHSV PORTAL_HSV = CHSV(220, 85, 85); // used by shimmer

