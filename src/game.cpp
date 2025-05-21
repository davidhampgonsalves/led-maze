#include <cmath>
#include <vector>
#include <Arduino.h>
#include <FastLED.h>
#include <math.h>
#include <common.h>
#include <colors.h>

#include "Game.h"

const double WEIGHT = 10;
const double GRAVITY = 9;
const double FRICTION_COEFFICIENT = 0.03;
const int GAME_MAX_X = 6000;
const int GAME_MAX_Y = 50000;

Game game;

Game::Game() {
  accelX, accelY = 0;
  // todo: replace with level starting pt
  posX = 2200;
  posY = 3400;
}

void Game::updateAccel(double beta, double gamma) {
    double betaRad = (beta * M_PI) / 180.0;
    double gammaRad = (gamma * M_PI) / 180.0;

    double forceDueToGravity = WEIGHT * GRAVITY * std::sin(betaRad);
    double normalForce = WEIGHT * GRAVITY * std::cos(betaRad);
    double frictionForce = FRICTION_COEFFICIENT * normalForce;

    double netForceX = forceDueToGravity * std::cos(gammaRad) - frictionForce * std::cos(gammaRad);
    double netForceY = forceDueToGravity * std::sin(gammaRad) - frictionForce * std::sin(gammaRad);

    accelX = static_cast<int>(netForceX / WEIGHT);
    accelY = static_cast<int>(netForceY / WEIGHT);
}

void Game::update() {
  // apply current accel to posX/Y
  unsigned long now = millis();
  int elapsed = now - time;

  double secs = static_cast<double>(elapsed) / 1000;

  posX = posX + (secs * accelX);
  posY = posY + (secs * accelY);

  if(posX < 0) posX = 0;
  if(posX > GAME_MAX_X) posX = 0;
  if(posY < 0) posY = 0;
  if(posY > GAME_MAX_Y) posY = 0;

  time = now;
}

const int ADJ_LED_MAX_BRT = 150;
void Game::draw(CRGB leds[]) {
  int xIndex = x();
  int yIndex = y();
  double xRemainder = static_cast<double>(posX) / 1000 - xIndex;
  double yRemainder = static_cast<double>(posY) / 1000 - yIndex;
  int adjX = xRemainder < 0.5 ? -1 : 1;
  int adjY = yRemainder < 0.5 ? -1 : 1;

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

  setLed(xIndex, yIndex, CHSV(BALL_H, BALL_S, 150), leds);
  setLed(xIndex + adjX, yIndex, CHSV(BALL_H, BALL_S, ADJ_LED_MAX_BRT * xRemainder), leds);
  setLed(xIndex, yIndex + adjY, CHSV(BALL_H, BALL_S, ADJ_LED_MAX_BRT * yRemainder), leds);
  setLed(xIndex + adjX,  yIndex + adjY, CHSV(BALL_H, BALL_S, ADJ_LED_MAX_BRT * (xRemainder + yRemainder) / 2), leds);


  int c = 10;
  for(int x = 0 ; x < 6 ; x++) {
    for(int y = 0 ; y < MAX_Y ; y += 1) {
      setLed(x, y, CHSV(BALL_H, BALL_S, c), leds);
      c += 10;
      if(c > 255) return;
    }
  }
}


int Game::x() {
  return round(posX / 1000);
}

int Game::y() {
  return round(posY / 1000);
}