#include <Arduino.h>
#include <FastLED.h>
#include "common.h"
#include "animations.h"
#include "state.h"
#include "colors.h"

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

  for (int y = 0; y < MAX_Y ; y++) {
    for (int x = 0; x < MAX_X ; x++) {
      if(random(10) > 5) setLed(random(6), y, STAR_COLOR, leds);
    }
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

    if(frameCount % 4) {
      if(ballX <= 0) ballDirRight = true;
      if(ballX >= 2) ballDirRight = false;
      ballX += ballDirRight ? 1 : -1;
    }
    if(ballY > MAX_Y+5) ballY = 0;
    ballY++;
  }

  drawBall(ballX, ballY, leds);

  if(elapsed > 3000) setNextState(HIGH_SCORES);
}
