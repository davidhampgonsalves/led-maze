#include <Arduino.h>
#include "state.h"

static State state = START_UP;

static unsigned long stateStart;

void updateState(State s) {
  // why do I need this?
  // if(state == GAME_START && stateStart < 500) return; // if game just started ignore this state change

  stateStart = millis();
  state = s;
};
unsigned long getStateStart() { return stateStart; }
State curState() { return state; };