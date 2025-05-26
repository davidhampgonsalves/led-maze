#include <Arduino.h>
#include <FastLED.h>

#include "ControlServer.h"
#include "Game.h"
#include "text.h"
#include "state.h"

#define NUM_LEDS 300
#define DATA_PIN 14
CRGB leds[NUM_LEDS];

ControlServer *server;

void setup(){
  // Serial port for debugging purposes
  Serial.begin(115200);

  Serial.println("Starting up");

  server = new ControlServer();
  server->connect();

  FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);  // GRB ordering is assumed
  FastLED.setBrightness(100);
  Serial.println("Start up complete");
}

static uint32_t lastWS = 0;
static uint32_t deltaWS = 2000;
static unsigned long time;

void title(unsigned long elapsed) {
  write("maze", leds);
}

void title(unsigned long elapsed) {
  if(elapsed < WORD_WAIT) write("get", leds);
  else if(elapsed < WORD_WAIT * 2) write("ready", leds);
  else updateState(PLAYING);
}

void play(unsigned long elapsed) {
  game.update(elapsed);
  game.draw(leds);
  // level->draw();
}

void loop() {
  unsigned long now = millis();
  int elapsed = now - time;
  FastLED.clear();

  if (elapsed >= deltaWS) {
    server->cleanupWsClients();
    lastWS = millis();
  }

  if(state == TITLE) title(elapsed);
  if(state == GAME_START) game_start(elapsed);
  if(state == PLAYING) play(elapsed);

  // delay(30); // TODO: maintain FPS
  FastLED.show();
  time = now;
}