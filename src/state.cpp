#include <Arduino.h>
#include "state.h"

static State cur = START_UP;
static State next = START_UP;
static bool isNewState = false;
static unsigned long lastInputTime;

static unsigned long stateStart;

void setNextState(State s) {
  next = s;
}

void transitionState() {
  if(cur != next) {
    cur = next;
    stateStart = millis();
    isNewState = true;
  } else if(isNewState)
    isNewState = false;
}

bool isFreshState() { return isNewState; }
unsigned long getStateStart() { return stateStart; }
State curState() { return cur; }

void trackLastInput() {
  lastInputTime =  millis();
}

unsigned long getLastInputTime() {
  return lastInputTime;
}