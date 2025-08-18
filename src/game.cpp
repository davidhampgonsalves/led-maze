#include <Arduino.h>
#include <FastLED.h>
#include <cmath>
#include <colors.h>
#include <common.h>
#include <math.h>
#include <state.h>
#include "ControlServer.h"
#include <text.h>
#include <vector>

#include "Game.h"
#include "Level.h"

const int GAME_MAX_X = 6000;
const int GAME_MAX_Y = 50000;
const double BOUNCE = 0.6;
const int LEVEL_TIME_BONUS = 10000;

Game game; // global game

Game::Game() {
  GRAVITY = 9.81;
  FRICTION = 0.006;
  ACCEL_DIVISOR = 160;
  TERM_VELOCITY = 50;
  SLOW_TERM_VELOCITY = 5;
  SLOW_FRICTION = 0.02;
}

void Game::start(int lvl, bool isRestart) {
  level = new Level(lvl);
  accelX, accelY = 0;

  if (lvl == 1 && !isRestart) {
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
    server.playSong("music/sparky.wav");
    break;
  case 2:
    server.playSong("music/spooky.wav");
    break;
  case 3:
    server.playSong("music/choir.wav");
    break;
  default:
    server.playSong("music/choir.wav");
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

  double termVel = TERM_VELOCITY;
  if(level->at(x, y) == SLOW)
    termVel = SLOW_TERM_VELOCITY;
  velX = limitVelocity(velX, termVel);
  velY = limitVelocity(velY, termVel);

  int prevX = x;
  int prevY = y;
  int prevPosX = posX;
  int prevPosY = posY;

  posX = posX + interval * velX;
  posY = posY + interval * velY;

  x = posToIndex(posX);
  y = posToIndex(posY);

  checkCollisions(prevX, prevY, prevPosX, prevPosY);
}

void Game::draw(CRGB leds[]) {
  State state = curState();
  auto elapsed = millis() - getStateStart(); // get this fresh since update can cause state changes

  switch (state) {
  case(PLAYING_LEVEL_END):
    return drawLevelEnd(elapsed, leds);
  case(PLAYING_LOSE_LIFE):
    return drawLoseLife(elapsed, leds);
  }

  level->draw(elapsed, leds);

  setLed(x, y, BALL_COLOR, leds);

  switch (state) {
  case (PLAYING_LEVEL_START):
    drawLevelStart(elapsed, leds);
    break;
  case (PLAYING_DEATH):
    drawDeath(elapsed, leds);
    break;
  };
}

void Game::drawLevelStart(unsigned long elapsed, CRGB leds[]) {
  server.playSound("sounds/spawn.wav");
  animateRing(elapsed, START_BURST, PLAYING, false, x, y, leds);
}

void Game::drawDeath(unsigned long elapsed, CRGB leds[]) {
  animateRing(elapsed, DEATH_BURST, PLAYING_LOSE_LIFE, true, x, y, leds);
}

void Game::drawLoseLife(unsigned long elapsed, CRGB leds[]) {
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

static long prevScore;
void Game::drawLevelEnd(unsigned long elapsed, CRGB leds[]) {
  if (elapsed == 0) {
    server.stopSong();
    prevScore = score;
    score += 100;
    Serial.println("level timer:");
    Serial.println(levelTimer);
    // TODO: add time bonus (broken)
    if (levelTimer < LEVEL_TIME_BONUS)
      score += (LEVEL_TIME_BONUS - levelTimer) / 100;
  }

  if (elapsed < 4000) {
    int countUp = prevScore;
    if (elapsed > 700 && elapsed < 1700) {
      countUp = ((static_cast<float>(elapsed - 700) / 1000.0) * (score - prevScore)) + prevScore;
      server.playSound("sounds/count.wav");
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

void Game::checkCollisions(int prevX, int prevY, int prevPosX, int prevPosY) {
  if (x == prevX && y == prevY) return; // haven't entered new px

  int px = level->at(x, y);
  int prevPx = level->at(prevX, prevY);

  // Serial.printf("%d/%d-vs %d/%d\n", x, y, prevX, prevY);
  switch (px) {
  case EMPTY:
    checkDiags(prevX, prevY, prevPosX, prevPosY);
    break;
  case WALL:
    wall(prevX, prevY, prevPosX, prevPosY);
    break;
  case FINISH:
    server.playSound("sounds/win.wav");
    updateState(PLAYING_LEVEL_END);
    break;
  case FIRE:
    server.playSound("sounds/death.wav");
    updateState(PLAYING_DEATH);
    break;
  case SLOW:
    server.playSound("sounds/slow.wav");
    break;
  case PORTAL:
    bool canWarp = warp(prevX, prevY);
    if (!canWarp)
      wall(prevX, prevY, prevPosX, prevPosY);
    break;
  }
}


void Game::checkDiags(int prevX, int prevY, int prevPosX, int prevPosY) {
  if (x == prevX || y == prevY) return;

  int adjX = (x > prevX ? -1 : 1) + x;
  int adjY = (y > prevY ? -1 : 1) + y;
  if (level->at(adjX, y) == WALL && level->at(x, adjY) == WALL) {
    x = prevX;
    y = prevY;

    posX = prevPosX;
    posY = prevPosY;

    velX = -BOUNCE * velX;
    velY = -BOUNCE * velY;
  }
}

void Game::wall(int prevX, int prevY, int prevPosX, int prevPosY) {
  bool impact = false;
  if (level->at(x, prevY) == WALL) {
    velX = -BOUNCE * velX;
    x = prevX;
    posX = prevPosX;
    if(abs(velX) > 10) {
      impact = true;
      server.playSound("sounds/bounce.wav");
    } else if(abs(velX) > 3)
      server.playSound("sounds/sm-bounce.wav");
  }
  if (level->at(prevX, y) == WALL) {
    velY = -BOUNCE * velY;
    y = prevY;
    posY = prevPosY;
    if(abs(velY) > 10) {
      impact = true;
      server.playSound("sounds/bounce.wav");
    } else if(abs(velY) > 3)
      server.playSound("sounds/sm-bounce.wav");
  }

  if (impact && level->isPx(x, y, BREAKABLE_WALL)) level->breakPx(x, y);
}

// warps only look in y axis for a single sibling warp
bool Game::warp(int prevX, int prevY) {
  int warpY = -1;
  Serial.println("Warp called");
  for(int tmpY = 0 ; tmpY < MAX_Y && warpY < 0 ; tmpY++) {
    if(tmpY == y) continue;
    if(level->at(x, tmpY) == PORTAL) warpY = tmpY;
  }

  Serial.println("Warp found");
  Serial.println(warpY);
  if(warpY < 0) return false;

  int offsetX = x > prevX ? 500 : -500;
  int offsetY = y > prevY ? 500 : -500;

  unsigned long tmpXPos = posX + offsetX;
  unsigned long tmpYPos = indexToPos(warpY) + offsetY;

  Serial.println("Warping to");
  Serial.print(x);
  Serial.print("/");
  Serial.println(warpY);
  Serial.print(tmpXPos);
  Serial.println(tmpYPos);
  if (level->at(posToIndex(tmpXPos), posToIndex(tmpYPos)) == EMPTY) {
    updatePos(tmpXPos, tmpYPos);
    server.playSound("sounds/portal.wav");
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