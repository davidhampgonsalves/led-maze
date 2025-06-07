#include <Arduino.h>
#include "state.h"

static State state = TITLE;

static unsigned long stateStart;

void updateState(State s) {
  if(state == GAME_START && stateStart < 500) return; // if game just started ignore this state change

  stateStart = millis();
  state = s;
};
unsigned long getStateStart() { return stateStart; }
State curState() { return state; };