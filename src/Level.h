#pragma once

#include <FastLED.h>
#include <common.h>

enum Px { EMPTY, WALL, FIRE, FINISH, START, PORTAL, SLIME, H_WALL, H_FIRE, H_FINISH, H_PORTAL };

const uint8_t LEVEL_COUNT = 3;

const CRGB EMPTY_RGB = CRGB(0, 0, 0);
const CRGB FIRE_RGB = CRGB(255, 0, 0);
const CRGB START_RGB = CRGB(255, 0, 255);
const CRGB FINISH_RGB = CRGB(0, 255, 0);
const CRGB PORTAL_RGB = CRGB(0, 101, 255);

const CRGB H_FIRE_RGB = CRGB(1, 0, 0);
const CRGB H_WALL_RGB = CRGB(1, 1, 1);
const CRGB H_FINISH_RGB = CRGB(0, 1, 0);
const CRGB H_PORTAL_RGB = CRGB(0, 0, 1);

class Level {
  public:
    Level(const char *path);
    Level(int num);
    void load(const char *path);
    void update(int interval);
    void draw(unsigned long elapsed, CRGB leds[]);
    Pt find(int x, int y, Px px);
    Px at(int x, int y);
    bool isHidden(CRGB c);
    int levelNum;
    int startX;
    int startY;

  private:
    CRGB level[MAX_X * MAX_Y];
};