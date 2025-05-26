#include <cmath>
#include <vector>
#include <Arduino.h>
#include <FastLED.h>
#include <math.h>
#include <common.h>
#include <colors.h>

#include "Game.h"

const int GAME_MAX_X = 6000;
const int GAME_MAX_Y = 50000;

Game game;

Game::Game() {
  accelX, accelY = 0;
  // todo: replace with level starting pt
  posX = 3000;
  posY = 4000;
  velX = 0;
  velY = 0;
}

const double GRAVITY = 9.81;  // m/s²
void Game::updateAccel(double beta, double gamma) {
    double betaRad = beta * M_PI / 180.0;
    double gammaRad = gamma * M_PI / 180.0;

    accelY = -static_cast<int>(GRAVITY * sin(betaRad)) * 5;
    accelX = static_cast<int>(GRAVITY * sin(gammaRad)) * 5;

    // Serial.printf("accel x: %d, y: %d\n", accelX, accelY);
}

// TODO: pass in elapsed from main loop
void Game::update(int elapsed) {
  // TODO: need to add velocityX/Y and add accel to that and that is what gets added to pos
  // Does friction reduce velocity?
  // apply current accel to posX/Y

  velX = elapsed * accelX;
  velY = elapsed * accelY;
  posX = posX + elapsed * velX;
  posY = posY + elapsed * velY;

  // TODO: this needs to be generalized for wall detection
  // track lastX/Y and when this update will cause a change
  // do this check
  if(posX < 0 || posX > GAME_MAX_X-1000) {
    if(posX < 0) posX = 0;
    else posX = GAME_MAX_X-1000;
    velX = -0.6 * velX;
  }
  if(posY < 0 || posY > GAME_MAX_Y-1000) {
    if(posY < 0) posY = 0;
    else posY = GAME_MAX_Y-1000;
    velY = -0.6 * velY;
  }

  x = calcX();
  y = calcY();

  Serial.printf("pos x: %d, y: %d\n", posX, posY);
}

const int ADJ_LED_MAX_BRT = 100;
void Game::draw(CRGB leds[]) {
  double xRemainder = static_cast<double>(posX) / 1000 - x;
  double yRemainder = static_cast<double>(posY) / 1000 - y;
  int adjX = xRemainder < 0.5 ? -1 : 1;
  int adjY = yRemainder < 0.5 ? -1 : 1;


  // adjacent ball pixels
  // setLed(xIndex, yIndex, CHSV(BALL_H, BALL_S, 150), leds);
  // setLed(xIndex + adjX, yIndex, CHSV(BALL_H, BALL_S, ADJ_LED_MAX_BRT * xRemainder), leds);
  // setLed(xIndex, yIndex + adjY, CHSV(BALL_H, BALL_S, ADJ_LED_MAX_BRT * yRemainder), leds);
  // setLed(xIndex + adjX,  yIndex + adjY, CHSV(BALL_H, BALL_S, ADJ_LED_MAX_BRT * (xRemainder + yRemainder) / 3), leds);

  // Serial.print("pos: ");
  // Serial.print(xIndex);
  // Serial.print(", ");
  // Serial.print(yIndex);
  // Serial.print(" - ");
  // Serial.print(xRemainder);
  // Serial.print(", ");
  // Serial.println(yRemainder);

  // Serial.print("led: ");
  // Serial.print(xIndex * MAX_Y + yIndex);
  // Serial.print(", ");
  // Serial.print(((xIndex + adjX) * MAX_Y) + yIndex);
  // Serial.print(", ");
  // Serial.print((xIndex * MAX_Y) + (yIndex + adjY));
  // Serial.print(", ");
  // Serial.println(((xIndex + adjX) * MAX_Y) + (yIndex + adjY));

  // display fade example row
  // int c = 10;
  // for(int x = 0 ; x < 6 ; x++) {
  //   for(int y = 0 ; y < MAX_Y ; y += 1) {
  //     setLed(x, y, CHSV(BALL_H, BALL_S, c), leds);
  //     c += 10;
  //     if(c > 255) return;
  //   }
  // }
}


int Game::calcX() {
  return round(posX / 1000);
}

int Game::calcY() {
  return round(posY / 1000);
}