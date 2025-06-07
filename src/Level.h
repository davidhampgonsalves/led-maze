#pragma once

#include <FastLED.h>
#include <common.h>

enum Px {
  EMPTY, WALL, FIRE, FINISH, START,
};

const uint8_t LEVEL_COUNT = 3;

const CRGB EMPTY_RGB = CRGB(0, 0, 0);
const CRGB FIRE_RGB = CRGB(255, 0, 0);
const CRGB START_RGB = CRGB(255, 0, 255);
const CRGB FINISH_RGB = CRGB(0, 255, 0);

class Level{
  public:
    Level(const char* path);
    Level(int num);
    void load(const char* path);
    void update(int elapsed);
    void draw(CRGB leds[]);
    Px at(int x, int y);
    int levelNum;
    int startX;
    int startY;
  private:
    CRGB level[MAX_X * MAX_Y];
};