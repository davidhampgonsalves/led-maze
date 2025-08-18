#pragma once

#include "Level.h"
#include <FastLED.h>

class Game{
  public:
    Game();
    void updateAccel(double, double);
    void update(int elapsed);
    void draw(CRGB leds[]);
    void start(int lvl, bool isRestart);

    double GRAVITY;
    double FRICTION;
    double SLOW_FRICTION;
    double TERM_VELOCITY;
    double SLOW_TERM_VELOCITY;
    int ACCEL_DIVISOR;
    Level *level;
    int x, y;
    int posX, posY;
    long score;
  private:
    double applyFriction(unsigned long elapsed, double vel);
    void checkCollisions(int prevX, int prevY, int prevPosX, int prevPosY);
    void checkDiags(int prevX, int prevY, int prevPosX, int prevPosY);
    bool warp(int prevX, int prevY);
    void wall(int prevX, int prevY, int prevPosX, int prevPosY);

    void drawLevelStart(unsigned long elapsed, CRGB leds[]);
    void drawLevelEnd(unsigned long elapsed, CRGB leds[]);
    void drawDeath(unsigned long elapsed, CRGB leds[]);
    void drawLoseLife(unsigned long elapsed, CRGB leds[]);

    void updatePos(unsigned long newXPos, unsigned long newYPos);
    int speed();

    unsigned long levelTimer;
    unsigned int lives;
    double accelX, accelY;
    double velX, velY;
    int lastX, lastY;
};

extern Game game;
