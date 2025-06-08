#include <Arduino.h>
#include <FastLED.h>
#include <cmath>
#include <colors.h>
#include <common.h>
#include <math.h>
#include <state.h>
#include <vector>

#include "Game.h"
#include "Level.h"

const int GAME_MAX_X = 6000;
const int GAME_MAX_Y = 50000;
const double BOUNCE = 0.6;

Game game; // global game

Game::Game() {
  GRAVITY = 9.81;
  FRICTION = 0.002;
  ACCEL_DIVISOR = 160;
  TERM_VELOCITY = 50;
}

void Game::start(int lvl) {
  level = new Level(lvl);
  accelX, accelY = 0;

  score = 0;

  posX = indexToPos(level->startX);
  posY = indexToPos(level->startY);
  x = posToIndex(posX);
  y = posToIndex(posY);

  velX = 0;
  velY = 0;
}

void Game::updateAccel(double beta, double gamma) {
  double betaRad = beta * M_PI / 180.0;
  double gammaRad = gamma * M_PI / 180.0;

  accelY = -1 * GRAVITY * sin(betaRad) / ACCEL_DIVISOR;
  accelX = GRAVITY * sin(gammaRad) / (ACCEL_DIVISOR + 10);
}

double Game::applyFriction(unsigned long elapsed, double vel) {
  double fric = elapsed * FRICTION * (vel > 0 ? -1 : 1);
  double nextVel = vel + fric;

  if ((vel >= 0 && nextVel < 0) || (vel < 0 && nextVel > 0))
    return 0;

  return vel + fric;
}

void Game::update(int elapsed) {
  if (curState() != PLAYING)
    return;

  level->update(elapsed);

  velX = applyFriction(elapsed, velX);
  velY = applyFriction(elapsed, velY);

  velX += elapsed * accelX;
  velY += elapsed * accelY;

  if (velX > TERM_VELOCITY)
    velX = TERM_VELOCITY;
  if (velX < -TERM_VELOCITY)
    velX = -TERM_VELOCITY;
  if (velY > TERM_VELOCITY)
    velY = TERM_VELOCITY;
  if (velY < -TERM_VELOCITY)
    velY = -TERM_VELOCITY;

  int prevX = x;
  int prevY = y;
  double prevPosX = posX;
  double prevPosY = posY;

  posX = posX + elapsed * velX;
  posY = posY + elapsed * velY;
  x = posToIndex(posX);
  y = posToIndex(posY);

  checkCollisions(prevX, prevY, prevPosX, prevPosY);
}

void Game::checkCollisions(int prevX, int prevY, double prevPosX,
                           double prevPosY) {
  int px = level->at(x, y);
  int prevPx = level->at(prevX, prevY);

  if (x == prevX && y == prevY && px == prevPx)
    return; // nothing has changed

  // TODO: need to handle portal, impact, kill, bouncy wall
  // TODO: can you be crushed?
  switch (px) {
    case EMPTY:
      checkDiags(prevX, prevY);
      break;
    case WALL:
      wall(prevX, prevY, prevPosX, prevPosY);
      break;
    case FINISH:
      updateState(LEVEL_END);
      break;
    case FIRE:
      updateState(PLAYING_DEATH);
      break;
    case PORTAL:
      bool canWarp = warp(prevX, prevY);
      if(!canWarp) wall(prevX, prevY, prevPosX, prevPosY);
      break;
  }
}

void Game::draw(unsigned long elapsed, CRGB leds[]) {
  level->draw(leds);

  setLed(x, y, BALL_COLOR, leds);

  switch (curState()) {
    case (LEVEL_START):
      levelStart(elapsed, leds);
      break;
    case (LEVEL_END):
      levelEnd(elapsed, leds);
      break;
    case (PLAYING_DEATH):
      death(elapsed, leds);
      break;
  };
}

void Game::levelStart(unsigned long elapsed, CRGB leds[]) {
  animateRing(elapsed, START_BURST, PLAYING, false, x, y, leds);
}

void Game::levelEnd(unsigned long elapsed, CRGB leds[]) {
  if (level->levelNum >= LEVEL_COUNT)
    updateState(GAME_END);
  else {
    updateState(LEVEL_START);
    start(level->levelNum + 1);
  }
}

void Game::death(unsigned long elapsed, CRGB leds[]) {
  animateRing(elapsed, DEATH_BURST, DEAD, true, x, y, leds);
}

void Game::checkDiags(int prevX, int prevY) {
  // need to check diags to see if allowed
  if (x == prevX || y == prevY)
    return; // not diag

  int adjX = (x > prevX ? -1 : 1) + x;
  int adjY = (y > prevY ? -1 : 1) + y;
  if (level->at(adjX, y) == WALL && level->at(x, adjY) == WALL) {
    x = prevX;
    velX = -BOUNCE * velX;

    y = prevY;
    velY = -BOUNCE * velY;
  }
}

void Game::wall(int prevX, int prevY, int prevPosX, int prevPosY) {
  if (x != prevX) {
    x = prevX;
    posX = prevPosX;
    velX = -BOUNCE * velX;
  }
  if (y != prevY) {
    y = prevY;
    posY = prevPosY;
    velY = -BOUNCE * velY;
  }
}

bool Game::warp(int prevX, int prevY) {
  Pt target = level->find(x, y, PORTAL);

  int offsetX = x > prevX ? 501 : -501;
  int offsetY = y > prevY ? 501 : -501;

  unsigned long tmpXPos = indexToPos(target.x) + offsetX;
  unsigned long tmpYPos = indexToPos(target.y) + offsetY;

  if(level->at(posToIndex(tmpXPos), posToIndex(tmpYPos)) == EMPTY) {
    updatePos(tmpXPos, tmpYPos);
    return true;
  } else {
    return false;
  }
}

void Game::updatePos(unsigned long newPosX, unsigned long newPosY) {
  posX = newPosX;
  posY = newPosY;
  x = posToIndex(posX);
  y = posToIndex(posY);
}