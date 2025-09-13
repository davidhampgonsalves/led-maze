#include <Arduino.h>
#include "state.h"

static State cur = START_UP;
static State prev = START_UP;

static unsigned long stateStart;

void updateState(State s) {
  stateStart = millis();
  prev = cur;
  cur = s;
};
unsigned long getStateStart() { return stateStart; }
State curState() { return cur; };
State prevState() { return prev; };
void updatePrevState(State s) { prev = s; }