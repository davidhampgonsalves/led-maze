#pragma once

enum State {
  START_UP,
  TITLE,
  GAME_START,
  GAME_OVER,
  GAME_WIN,
  PLAYING,
  PLAYING_DEATH,
  PLAYING_LOSE_LIFE,
  PLAYING_LEVEL_START,
  PLAYING_LEVEL_END,
  HIGH_SCORE,
  HIGH_SCORES,
  DEMO,
  RACING_ANIMATION,
};

void setNextState(State s);
void transitionState();
unsigned long getStateStart();
bool isFreshState();
State curState();

void trackLastInput();
unsigned long getLastInputTime();