#pragma once

#include "Level.h"
#include <FastLED.h>

class Game{
  public:
    Game();
    void updateAccel(double, double);
    void update(int elapsed);
    void draw(unsigned long elapsed, CRGB leds[]);
    void start(int lvl);

    double GRAVITY;
    double FRICTION;
    double TERM_VELOCITY;
    int ACCEL_DIVISOR;
    uint score;
    Level *level;
  private:
    double applyFriction(unsigned long elapsed, double vel);
    void checkCollisions(int prevX, int prevY, double prevPosX, double prevPosY);
    void checkDiags(int prevX, int prevY);
    void levelStart(unsigned long elapsed, CRGB leds[]);
    void levelEnd(unsigned long elapsed, CRGB leds[]);


    double accelX, accelY;
    double velX, velY;
    int posX, posY;
    int lastX, lastY;
    int x, y;

    int calcX();
    int calcY();
};

extern Game game;
