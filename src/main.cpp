#include <Arduino.h>
#include <FastLED.h>
#include <vector>

#include "ControlServer.h"
#include "Game.h"
#include "text.h"
#include "state.h"
#include "file.h"

#define NUM_LEDS 300
#define DATA_PIN 25
#define RELAY_PIN 27
CRGB leds[NUM_LEDS];

ControlServer *server;
std::vector<HighScore> scores;
Level *titleLevel = NULL;

void setup(){
  Serial.begin(115200);
  Serial.println("Starting up.");

  server = new ControlServer();
  server->connect();
  Serial.println("Server is connected.");

  FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);  // GRB ordering is assumed
  FastLED.setBrightness(30);
  FastLED.setTemperature(0xFF7029);

  FastLED.clear();
  FastLED.show();

  scores = readHighScores();
  Serial.println("SD card read complete.");

  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, HIGH);

  titleLevel = new Level("/title.bmp");

  updateState(TITLE);

  Serial.println("Start up complete.");
}

static uint32_t lastWS = 0;
static uint32_t deltaWS = 2000;
static unsigned long prev;

void gameStart(unsigned long elapsed) {
  // Serial.printf("%d, %d %d\n", elapsed, now, getStateStart());
  if(elapsed < WORD_WAIT) write("get", leds);
  else if(elapsed < WORD_WAIT * 2) write("ready", leds);
  else {
    game.start(1);
    updateState(LEVEL_START);
  }
}

void gameEnd(unsigned long elapsed) {
  if(elapsed < WORD_WAIT) write("you", leds);
  else if(elapsed < WORD_WAIT * 2) write("win", leds);
  else updateState(TITLE);
}

void title(unsigned long elapsed) {
  titleLevel->update(elapsed);
  titleLevel->draw(leds);
  return;

  if(elapsed < 3000) {
    titleLevel->update(elapsed);
    titleLevel->draw(leds);
  } else
    updateState(HIGH_SCORES);
}

int scoreIndex = 0;
void highScores(unsigned long now) {
  auto elapsed = now - getStateStart();
  if(elapsed < WORD_WAIT * 3) {
    if(elapsed < WORD_WAIT) {
      scoreIndex = 0;
      write("hall", leds);
    } else if(elapsed < WORD_WAIT * 2)
      write("of", leds);
    else
      write("fame", leds);

    return;
  }

  elapsed -= WORD_WAIT * 3 + (scoreIndex * WORD_WAIT_LONG * 2);
  if(elapsed < WORD_WAIT_LONG) write(scores[scoreIndex].name, leds);
  else if(elapsed < WORD_WAIT_LONG * 2) write(scores[scoreIndex].score, leds);
  else {
    scoreIndex += 1;
    if(scoreIndex >= scores.size()) updateState(TITLE);
  }
}

void play(uint interval, unsigned long elapsed) {
  game.update(interval);

  game.draw(elapsed, leds);
}

void loop() {
  unsigned long now = millis();
  uint interval = now - prev;
  auto elapsed = now - getStateStart();
  FastLED.clear();

  if (elapsed >= deltaWS) {
    server->cleanupWsClients();
    lastWS = millis();
  }

  int state = curState();
  switch(state) {
    case TITLE:
      title(elapsed);
      break;
    case HIGH_SCORES:
      highScores(now);
      break;
    case GAME_START:
      gameStart(elapsed);
      break;
    case GAME_END:
      gameEnd(elapsed);
      break;
    case PLAYING:
    case PLAYING_DEAD:
    case LEVEL_START:
    case LEVEL_END:
      play(interval, elapsed);
      break;
    default:
      Serial.println("ERROR: game state not handled.");
  }

  // TODO: display frame time every 5 seconds
  FastLED.show();
  prev = now;
}