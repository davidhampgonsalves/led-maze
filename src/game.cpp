#include <Arduino.h>
#include <FastLED.h>
#include <cmath>
#include <colors.h>
#include <common.h>
#include <math.h>
#include <state.h>
// #include <stdio.h>
#include <text.h>
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

  if (lvl == 1) {
    score = 0;
    lives = 3;
  }
  levelTimer = 0;

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
  if (level->isPx(x, y, SLOW))
    fric *= 3;
  double nextVel = vel + fric;

  if ((vel >= 0 && nextVel < 0) || (vel < 0 && nextVel > 0))
    return 0;

  return vel + fric;
}

void Game::update(int interval) {
  if (curState() != PLAYING)
    return;

  levelTimer += interval;
  level->update(interval);

  velX = applyFriction(interval, velX);
  velY = applyFriction(interval, velY);

  velX += interval * accelX;
  velY += interval * accelY;

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

  posX = posX + interval * velX;
  posY = posY + interval * velY;
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

  // TODO: can you be crushed?
  switch (px) {
  case EMPTY:
    checkDiags(prevX, prevY);
    break;
  case WALL:
    wall(prevX, prevY, prevPosX, prevPosY);
    break;
  case FINISH:
    updateState(PLAYING_LEVEL_END);
    break;
  case FIRE:
    updateState(PLAYING_DEATH);
    break;
  case PORTAL:
    bool canWarp = warp(prevX, prevY);
    if (!canWarp)
      wall(prevX, prevY, prevPosX, prevPosY);
    break;
  }
}

void Game::draw(unsigned long elapsed, CRGB leds[]) {
  if (curState() == PLAYING_LEVEL_END)
    return levelEnd(elapsed, leds);

  if (curState() == PLAYING_LOSE_LIFE)
    return loseLife(elapsed, leds);

  level->draw(elapsed, leds);

  setLed(x, y, BALL_COLOR, leds);

  switch (curState()) {
  case (PLAYING_LEVEL_START):
    levelStart(elapsed, leds);
    break;
  case (PLAYING_DEATH):
    death(elapsed, leds);
    break;
  };
}

void Game::levelStart(unsigned long elapsed, CRGB leds[]) {
  animateRing(elapsed, START_BURST, PLAYING, false, x, y, leds);
}

static long prevScore;
void Game::levelEnd(unsigned long elapsed, CRGB leds[]) {
  if (elapsed == 0) {
    prevScore = score;
    score += 100;
    Serial.println(levelTimer);
    // TODO: add time bonus (broken)
    // long timeBonus = (10000 - levelTimer) / 100;
    // if (timeBonus > 0)
    //   score += timeBonus;
  }

  if (elapsed < 4000) {
    int countUp = prevScore;
    if (elapsed > 700 && elapsed < 1700)
      countUp =
          ((static_cast<float>(elapsed - 700) / 1000.0) * (score - prevScore)) +
          prevScore;
    if (elapsed >= 1700)
      countUp = score;

    writeFixed5(countUp, leds);
  } else if (level->levelNum >= LEVEL_COUNT)
    updateState(GAME_WIN);
  else {
    updateState(PLAYING_LEVEL_START);
    start(level->levelNum + 1);
  }
}

void Game::death(unsigned long elapsed, CRGB leds[]) {
  if (elapsed > 0) // HACK
    animateRing(elapsed, DEATH_BURST, PLAYING_LOSE_LIFE, true, x, y, leds);
}

void Game::loseLife(unsigned long elapsed, CRGB leds[]) {
  if (lives == 0)
    return updateState(GAME_OVER);

  if (elapsed < 700)
    write(lives, leds);
  else if (elapsed < 1500)
    write(lives - 1, leds);
  else {
    lives -= 1;
    updateState(PLAYING_LEVEL_START);
    start(level->levelNum);
  }
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

const int GLANCING_POS = 200;
void Game::wall(int prevX, int prevY, int prevPosX, int prevPosY) {
  int collisionPosX = x % PX_SIZE;
  int collisionPosY = y % PX_SIZE;
  bool isOverCenterX = collisionPosX > PX_CENTER;
  bool isOverCenterY = collisionPosY > PX_CENTER;

  if (level->isPx(x, y, BREAKABLE_WALL)) {
    // TODO: min speed check?
    level->breakPx(x, y);
  }

  if (x != prevX) {
    int absPos = isOverCenterX ? PX_SIZE - collisionPosX : collisionPosX;
    if (absPos < GLANCING_POS &&
        ((isOverCenterY && level->at(x, y + 1) != WALL) ||
         (!isOverCenterY && level->at(x, y - 1) != WALL)) &&
        ((isOverCenterX && level->at(x + 1, y) != WALL) ||
         (!isOverCenterX && level->at(x - 1, y) != WALL))) {
      double glancingRatio = absPos / GLANCING_POS;
      velX = BOUNCE * velX * (1 - glancingRatio);
      velY += (isOverCenterX ? 1 : -1) * BOUNCE * velX * glancingRatio;
    } else
      velX = -BOUNCE * velX;

    x = prevX;
    posX = prevPosX;
  }

  if (y != prevY) {
    int absPos = isOverCenterY ? PX_SIZE - collisionPosY : collisionPosY;
    if (absPos < GLANCING_POS &&
        ((isOverCenterX && level->at(x + 1, y) != WALL) ||
         (!isOverCenterX && level->at(x - 1, y) != WALL)) &&
        ((isOverCenterY && level->at(x, y + 1) != WALL) ||
         (!isOverCenterY && level->at(x, y - 1) != WALL))) {
      double glancingRatio = absPos / GLANCING_POS;
      velY = BOUNCE * velY * (1 - glancingRatio);
      velX += (isOverCenterY ? 1 : -1) * BOUNCE * velY * glancingRatio;
    } else
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

  if (level->at(posToIndex(tmpXPos), posToIndex(tmpYPos)) == EMPTY) {
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