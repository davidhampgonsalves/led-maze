#pragma once

enum State {
  TITLE,
  GAME_START,
  GAME_OVER,
  GAME_END,
  PLAYING,
  PLAYING_DEATH,
  DEAD,
  LEVEL_START,
  LEVEL_END,
  HIGH_SCORE,
  HIGH_SCORES,
  DEMO,
};

void updateState(State s);
unsigned long getStateStart();
State curState();