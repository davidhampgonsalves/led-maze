#pragma once

#include "Level.h"
#include <FastLED.h>

class Game{
  public:
    Game();
    void updateAccel(double, double);
    void update(int elapsed);
    void draw(unsigned long elapsed, CRGB leds[]);
    void start(int lvl, bool isRestart);

    double GRAVITY;
    double FRICTION;
    double TERM_VELOCITY;
    int ACCEL_DIVISOR;
    Level *level;
    int x, y;
    long score;
  private:
    double applyFriction(unsigned long elapsed, double vel);
    void checkCollisions(int prevX, int prevY, double prevPosX, double prevPosY);
    void checkDiags(int prevX, int prevY);
    void levelStart(unsigned long elapsed, CRGB leds[]);
    void levelEnd(unsigned long elapsed, CRGB leds[]);
    void death(unsigned long elapsed, CRGB leds[]);
    void loseLife(unsigned long elapsed, CRGB leds[]);
    bool warp(int prevX, int prevY);
    void wall(int prevX, int prevY, int prevPosX, int prevPosY);
    void updatePos(unsigned long newXPos, unsigned long newYPos);
    int speed();
    bool shouldGlance(int prevX, int prevY, bool isOverCenterX, bool isOverCenterY);

    unsigned long levelTimer;
    unsigned int lives;
    double accelX, accelY;
    double velX, velY;
    int posX, posY;
    int lastX, lastY;
};

extern Game game;
