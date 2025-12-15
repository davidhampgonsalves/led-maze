#include <Arduino.h>
#include <FastLED.h>
#include <cmath>
#include "screen.h"
#include "level.h"
#include "file.h"
#include <colors.h>
#include <common.h>
#include <math.h>
#include <Game.h>


Screen::Screen(const char* path) { load(path); }

void Screen::load(const char* path) {
  char data[2000];
  readFile(path, data);

  int headerOffset = static_cast<int>(data[10]);

  int dataIndex = headerOffset;
  for (int y = 0; y < MAX_Y; y++) {
    for (int x = 0; x < MAX_X; x++) {
      uint8_t b = static_cast<uint8_t>(data[dataIndex]);
      uint8_t g = static_cast<uint8_t>(data[dataIndex + 1]);
      uint8_t r = static_cast<uint8_t>(data[dataIndex + 2]);

      CRGB c = CRGB{r, g, b};
      screenData[x * MAX_Y + y] = c;

      if (x == MAX_X - 1) {
        dataIndex += 2; // skip padding: 6 pixels * 3 bytes to a multiple of 4
      }
      dataIndex += 3;
    }
  }
}

Px Screen::at(int x, int y) {
  CRGB c = colorAt(x, y, screenData);

  if (c == EMPTY_RGB || c == START_RGB)
    return EMPTY;
  else if(c == FINISH_RGB || c == H_FINISH_RGB)
    return FINISH;
  else if(c == FIRE_RGB || c == H_FIRE_RGB)
    return FIRE;
  else if(c == PORTAL_RGB || c == H_PORTAL_RGB)
    return PORTAL;
  else if(c == SLOW_RGB)
    return SLOW;

  return WALL;
}


void Screen::draw(unsigned long elapsed, CRGB leds[]) {
  for (int y = 0; y < MAX_Y; y++) {
    for (int x = 0; x < MAX_X; x++) {
      Px px = at(x, y);
      if(px == EMPTY) continue;

      CRGB c = colorAt(x, y, screenData);

      if(px == FIRE) {
        setFlameLed(x, y, leds);
      } else if(px == PORTAL)
        setShimmerLed(elapsed, x, y, PORTAL_HSV, leds);
      else if(px == SLOW)
        setShimmerLed(elapsed, x, y, SLOW_HSV, leds);
      else
        setLed(x, y, c, leds);
    }
  }
}
