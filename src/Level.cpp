#include <Arduino.h>
// #include <vector>
#include <FastLED.h>
#include <cmath>
#include "Level.h"
#include "file.h"
#include <colors.h>
#include <common.h>
#include <math.h>
#include <Game.h>

Level::Level(int num) {
  levelNum = num;
  char path[15];
  sprintf(path, "/levels/%02d.bmp", num);

  load(path);
}

Level::Level(const char* path) {
  load(path);
}

void Level::load(const char* path) {
  readFile(path, data);

  int headerOffset = static_cast<int>(data[10]);

  int dataIndex = headerOffset;
  for (int y = 0; y < MAX_Y; y++) {
    for (int x = 0; x < MAX_X; x++) {
      uint8_t b = static_cast<uint8_t>(data[dataIndex]);
      uint8_t g = static_cast<uint8_t>(data[dataIndex + 1]);
      uint8_t r = static_cast<uint8_t>(data[dataIndex + 2]);

      CRGB c = CRGB{r, g, b};
      if(c == START_RGB)
        startX = x, startY = y;

      level[x * MAX_Y + y] = c;

      if (x == MAX_X - 1) {
        dataIndex += 2; // skip padding: 6 pixels * 3 bytes to a multiple of 4
      }
      dataIndex += 3;
    }
  }
}

void Level::update(int interval) {}

void Level::draw(unsigned long elapsed, CRGB leds[]) {
  for (int y = 0; y < MAX_Y; y++) {
    for (int x = 0; x < MAX_X; x++) {
      Px px = at(x, y);
      if(px == EMPTY) continue;

      CRGB c = colorAt(x, y, level);
      if(isHidden(c)) {
        bool isClose = (x <= game.x + 1 && x >= game.x - 1 && y <= game.y + 1 && y >= game.y - 1);
        if(!isClose) continue;
      }

      if(px == FIRE) {
        setFlameLed(x, y, leds);
      } else if(px == PORTAL) {
        setPortalLed(elapsed, x, y, leds);
      } else if(c == H_WALL_RGB)
        setLed(x, y, HIDDEN_WALL_COLOR, leds);
      else
        setLed(x, y, c, leds);
    }
  }
}

bool Level::isHidden(CRGB c) {
  if(c == H_FIRE_RGB || c == H_FINISH_RGB || c == H_PORTAL_RGB || c == H_WALL_RGB)
    return true;
  return false;
}

Px Level::at(int x, int y) {
  CRGB c = colorAt(x, y, level);

  if (c == EMPTY_RGB || c == START_RGB)
    return EMPTY;
  else if(c == FINISH_RGB || c == H_FINISH_RGB)
    return FINISH;
  else if(c == FIRE_RGB || c == H_FIRE_RGB)
    return FIRE;
  else if(c == PORTAL_RGB || c == H_PORTAL_RGB)
    return PORTAL;

  return WALL;
}

bool Level::isPx(int x, int y, Px px) {
  CRGB c = colorAt(x, y, level);
  if(px == BREAKABLE_WALL && c == BREAKABLE_WALL_RGB)
    return true;
  else if(px == SLOW && c == SLOW_RGB)
    return true;
  else
    return false;
}

void Level::breakPx(int x, int y) {
  level[x * MAX_Y + y] = BLACK;
}

Pt Level::find(int x, int y, Px px) {
  for (int fY = 0; fY < MAX_Y; fY++) {
    for (int fX = 0 ; fX < MAX_X; fX++) {
      if(x == fX && y == fY) continue;

      if(at(fX, fY) == px)
        return Pt{fX, fY};
    }
  }
  return Pt{ -1, -1 };
}