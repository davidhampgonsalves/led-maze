#pragma once

#include <FastLED.h>
#include <common.h>

enum Px { EMPTY, WALL, BREAKABLE_WALL, FIRE, SLOW, FINISH, START, PORTAL, H_WALL, H_FIRE, H_FINISH, H_PORTAL };

const uint8_t LEVEL_COUNT = 14;

class Level {
  public:
    Level(const char *path);
    Level(int num);
    void load(const char *path);
    void update(int interval);
    void draw(unsigned long elapsed, CRGB leds[]);
    Pt find(int x, int y, Px px);
    Px at(int x, int y);
    bool isPx(int x, int y, Px px);
    void breakPx(int x, int y);
    bool isHidden(CRGB c);
    int levelNum;
    int startX;
    int startY;

  private:
    CRGB level[MAX_X * MAX_Y];
    char data[10000]; // todo: can be 2k
};