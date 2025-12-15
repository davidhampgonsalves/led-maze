#pragma once

#include <FastLED.h>
#include <common.h>
#include <level.h>

class Screen {
  public:
    Screen(const char *path);
    void load(const char *path);
    void draw(unsigned long elapsed, CRGB leds[]);
  private:
    CRGB screenData[MAX_X * MAX_Y];
    Px at(int x, int y);
};