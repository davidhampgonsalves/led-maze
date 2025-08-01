#include <Arduino.h>
#include <FastLED.h>
#include <cmath>
#include <colors.h>
#include <common.h>
#include <math.h>
#include <state.h>
#include <text.h>
#include <vector>
#include "audio.h"

#include "Game.h"
#include "Level.h"

const int GAME_MAX_X = 6000;
const int GAME_MAX_Y = 50000;
const double BOUNCE = 0.6;

Game game; // global game

Game::Game() {
  GRAVITY = 9.81;
  FRICTION = 0.006;
  ACCEL_DIVISOR = 160;
  TERM_VELOCITY = 50;
  SLOW_FRICTION = 0.9;
}

void Game::start(int lvl, bool isRestart) {
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

  if(isRestart) return;

  switch (lvl) {
  case 1:
    playSong("/music/sparky.wav");
    break;
  case 2:
    playSong("/music/spooky.wav");
    break;
  case 3:
    playSong("/music/choir.wav");
    break;
  default:
    playSong("/music/choir.wav");
  }
}

void Game::updateAccel(double beta, double gamma) {
  double betaRad = beta * M_PI / 180.0;
  double gammaRad = gamma * M_PI / 180.0;

  accelY = -1 * GRAVITY * sin(betaRad) / ACCEL_DIVISOR;
  accelX = GRAVITY * sin(gammaRad) / (ACCEL_DIVISOR + 10);
}

double Game::applyFriction(unsigned long elapsed, double vel) {
  double fricType = FRICTION;
  if (level->isPx(x, y, SLOW)) fricType = SLOW_FRICTION;

  double fric = elapsed * fricType * (vel > 0 ? -1 : 1);
  double nextVel = vel + fric;

  if ((vel >= 0 && nextVel < 0) || (vel < 0 && nextVel > 0))
    return 0;

  return vel + fric;
}

void Game::update(int interval) {
  if (curState() != PLAYING) return; // if not playing no updates needed

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

  if(posX < 0) posX = 0;
  if(posY < 0) posY = 0;

  x = posToIndex(posX);
  y = posToIndex(posY);

  checkCollisions(prevX, prevY, prevPosX, prevPosY);
}

void Game::checkCollisions(int prevX, int prevY, double prevPosX, double prevPosY) {
  if (x == prevX && y == prevY) return; // haven't entered new px

  int px = level->at(x, y);
  int prevPx = level->at(prevX, prevY);

  Serial.printf("%d/%d-vs %d/%d\n", x, y, prevX, prevY);
  switch (px) {
  case EMPTY:
    checkDiags(prevX, prevY);
    break;
  case WALL:
    wall(prevX, prevY, prevPosX, prevPosY);
    break;
  case FINISH:
    playSound("/sounds/win.wav");
    updateState(PLAYING_LEVEL_END);
    break;
  case FIRE:
    playSound("/sounds/death.wav");
    updateState(PLAYING_DEATH);
    break;
  case PORTAL:
    bool canWarp = warp(prevX, prevY);
    if (!canWarp)
      wall(prevX, prevY, prevPosX, prevPosY);
    break;
  }
}

void Game::draw(CRGB leds[]) {
  State state = curState();
  auto elapsed = millis() - getStateStart(); // get this fresh since update can cause state changes

  switch (state) {
  case(PLAYING_LEVEL_END):
    return levelEnd(elapsed, leds);
  case(PLAYING_LOSE_LIFE):
    return loseLife(elapsed, leds);
  }

  level->draw(elapsed, leds);

  setLed(x, y, BALL_COLOR, leds);

  switch (state) {
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
    stopSong();
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
    if (elapsed > 700 && elapsed < 1700) {
      countUp = ((static_cast<float>(elapsed - 700) / 1000.0) * (score - prevScore)) + prevScore;
      playSound("/sounds/count.wav");
    }
    if (elapsed >= 1700)
      countUp = score;

    writeFixed5(countUp, leds);
  } else if (level->levelNum >= LEVEL_COUNT)
    updateState(GAME_WIN);
  else {
    updateState(PLAYING_LEVEL_START);
    start(level->levelNum + 1, false);
  }
}

void Game::death(unsigned long elapsed, CRGB leds[]) {
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
    start(level->levelNum, true);
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

void Game::wall(int prevX, int prevY, int prevPosX, int prevPosY) {
  // check speed in direction that is hitting wall
  bool isImpactInX = x != prevX && level->at(x, prevY) == WALL;
  bool isImpactInY = y != prevY && level->at(prevX, y) == WALL;

  // Serial.printf("%d, %f, %d, %f\n", isImpactInX, abs(velX), isImpactInY, abs(velY));
  if ((isImpactInX && abs(velX) > 10) || (isImpactInY && abs(velY) > 10)) {
    playSound("/sounds/bounce.wav");
    if(level->isPx(x, y, BREAKABLE_WALL)) level->breakPx(x, y);
  }

  int collisionX = x % PX_SIZE;
  int collisionY = y % PX_SIZE;
  bool isOverCenterX = collisionX > PX_CENTER;
  bool isOverCenterY = collisionY > PX_CENTER;

  if (isImpactInX) {
    velX = -BOUNCE * velX;

    x = prevX;
    posX = prevPosX;
  }

  if (isImpactInY) {
    velY = -BOUNCE * velY;

    y = prevY;
    posY = prevPosY;
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
    playSound("/sounds/portal.wav");
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