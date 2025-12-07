#include <Arduino.h>
#include <FastLED.h>
#include "common.h"
#include "animations.h"
#include "state.h"
#include "colors.h"

unsigned long nextAnimationFrameAt = 0;
unsigned int frameCount;
unsigned int ballX;
unsigned int ballY;
bool dirRight = true;

void drawBall(int x, int y, CRGB* leds) {
  setLedIfBlank(x+1, y+1, BALL_COLOR, leds);
  setLedIfBlank(x+2, y+1, BALL_COLOR, leds);

  setLedIfBlank(x, y+2, BALL_COLOR, leds);
  setLedIfBlank(x+1, y+2, BALL_COLOR, leds);
  setLedIfBlank(x+2, y+2, BALL_COLOR, leds);
  setLedIfBlank(x+3, y+2, BALL_COLOR, leds);

  setLedIfBlank(x, y+3, BALL_COLOR, leds);
  setLedIfBlank(x+1, y+3, BALL_COLOR, leds);
  setLedIfBlank(x+2, y+3, BALL_HIGHLIGHT_COLOR, leds);
  setLedIfBlank(x+3, y+3, BALL_COLOR, leds);

  setLedIfBlank(x+1, y+4, BALL_COLOR, leds);
  setLedIfBlank(x+2, y+4, BALL_COLOR, leds);
}

void racingAnimationInit(CRGB* leds) {
  frameCount = 0;
  ballX = 1;
  ballY = 0;
  nextAnimationFrameAt = 0;

  for (int y = 0; y < MAX_Y ; y++) {
    if(random(10) > 7) setLed(random(6), y, STAR_COLOR, leds);
  }
}

void racingAnimation(unsigned long elapsed, CRGB* leds) {
  if(elapsed < nextAnimationFrameAt) return;

  frameCount++;

  nextAnimationFrameAt = elapsed + 40;

  for (int y = 0; y < MAX_Y ; y++) {
    for (int x = 0; x < MAX_X ; x++) {
      CRGB* c = getLed(x, y, leds);
      if(*c != BLACK && *c != BALL_COLOR && *c != BALL_HIGHLIGHT_COLOR && *c != BALL_SHADOW_COLOR) {
        setLed(x, y-1, *c, leds);
        setLed(x, y, BLACK, leds);
      }
    }
  }

  if(random(10) > 5) setLed(random(6), MAX_Y-1, STAR_COLOR, leds);

  if(frameCount % 2 == 0) {
    // clear ball pixels
    for (int x = 0; x < MAX_X ; x++) {
      for (int y = 0; y < MAX_Y ; y++) {
        CRGB* c = getLed(x, y, leds);
        if(*c == BALL_COLOR || *c == BALL_HIGHLIGHT_COLOR || *c == BALL_SHADOW_COLOR)
          setLed(x, y, BLACK, leds);
      }
    }

    if(frameCount % 4 == 0) {
      if(ballX <= 0) dirRight = true;
      if(ballX >= 2) dirRight = false;
      ballX += dirRight ? 1 : -1;
    }
    if(ballY > MAX_Y+5) setNextState(HIGH_SCORES);

    ballY++;
  }

  drawBall(ballX, ballY, leds);
}

unsigned int posX;
void rainbowAnimationInit() {
  frameCount = 0;
  dirRight = true;
  ballY = 0;
  posX = 0;
}

void drawRainbowRows(int startY, int offset, CRGB* leds) {
  for(int y=startY ; y > startY - 3 && y >= 0 ; y--) {
    setLed(offset, y, RED, leds);
    setLed(1 + offset, y, YELLOW, leds);
    setLed(2 + offset, y, GREEN, leds);
    setLed(3 + offset, y, BLUE, leds);
  }
}

void rainbowAnimation(unsigned long elapsed, CRGB* leds) {
  if(elapsed < nextAnimationFrameAt) return;

  FastLED.clear();

  frameCount++;

  nextAnimationFrameAt = elapsed + 50;

  if(ballY > MAX_Y+45) setNextState(GAME_WIN);

  ballY++;

  drawBall(1, ballY, leds);

  if(frameCount % 2 != 0) {
    if(posX <= 0) dirRight = true;
    if(posX >= 2) dirRight = false;
    posX += dirRight ? 1 : -1;
  }

  int offset = posX;
  bool offsetDirRight = dirRight;
  for (int y = ballY - 2; y >= 0 && ballY - y < 30; y-=3) {
    if(ballY - y <= 2) {
      drawRainbowRows(y, 1, leds);
      continue;
    }
    if(offset <= 0) offsetDirRight = true;
    if(offset >= 2) offsetDirRight = false;
    offset += offsetDirRight ? 1 : -1;

    // Serial.printf("y: %d, posX: %d, offset: %d\n", y, posX, offset);
    drawRainbowRows(y, offset, leds);
  }
}