#pragma once

#include <FastLED.h>

class Game{
  public:
    Game();
    void updateAccel(double, double);
    void update(int elapsed);
    void draw(CRGB leds[]);
  private:
    int accelX, accelY;
    int posX, posY;
    int velX, velY;
    int lastX, lastY;
    int x, y;

    int calcX();
    int calcY();
};

extern Game game;
