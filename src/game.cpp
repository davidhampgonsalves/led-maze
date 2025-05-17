#include <cmath>
#include <vector>
#include <Arduino.h>
#include <FastLED.h>
#include <math.h>

#include "Game.h";

Game::Game() {
  accelX, accelY = 0;
  // todo: replace with level starting pt
  posX = MAX_X / 2;
  posY = MAX_Y / 2;
}

const double WEIGHT = 10;
const double GRAVITY = 9;
const double FRICTION_COEFFICIENT = 0.03;
const int MAX_X = 6000;
const int MAX_Y = 50000;


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
  if(posX > MAX_X) posX = 0;
  if(posY < 0) posY = 0;
  if(posY > MAX_Y) posY = 0;

  time = now;
}

void Game::draw(CRGB leds[]) {
  int xIndex = x();
  int yIndex = y();
  double remainderX = posX - xIndex;
  double remainderY = posY - yIndex;
  int adjX = remainderX < 0.5 ? -1 : 1;
  int adjY = remainderY < 0.5 ? -1 : 1;

  leds[xIndex * yIndex] = CRGB(200, 200, 200);
  // TODO: scale brightness of adjacent pixels based on remainder values
  leds[(xIndex + adjX) * yIndex] = CRGB(0, 0, 200);
  leds[xIndex * (yIndex + adjY)] = CRGB(0, 0, 200);
  leds[(xIndex + adjX) * (yIndex + adjY)] = CRGB(0, 0, 200);
}

int Game::x() {
  return round(posX / 1000);
}

int Game::y() {
  return round(posY / 1000);
}