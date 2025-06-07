#pragma once

enum State {
  TITLE,
  GAME_START,
  GAME_END,
  PLAYING,
  LEVEL_START,
  LEVEL_END,
  PLAYING_DEAD,
  HIGH_SCORE,
  HIGH_SCORES,
  DEMO,
};

void updateState(State s);
unsigned long getStateStart();
State curState();