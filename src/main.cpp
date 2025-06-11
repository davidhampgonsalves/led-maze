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
Level *deadLevel = NULL;
Level *winLevel = NULL;

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
  deadLevel = new Level("/death.bmp");
  winLevel = new Level("/win.bmp");

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
    game.start(2);
    updateState(PLAYING_LEVEL_START);
  }
}

void gameEnd(unsigned long elapsed) {
  if(elapsed > 1500) {
    updateState(TITLE);
    return;
  }

  winLevel->update(0);
  winLevel->draw(elapsed, leds);
}

void gameOver(unsigned long elapsed) {
  if(elapsed > WORD_WAIT * 2)
    // TODO: switch to score animation (dedupe from levelEnd)
    // switch to highscore if you have a high score? - or just do something on the phone
    return updateState(TITLE);

  deadLevel->update(0);
  deadLevel->draw(elapsed, leds);
}

void title(unsigned long elapsed) {
  if(elapsed < 5000) {
    titleLevel->update(0);
    titleLevel->draw(elapsed, leds);
  } else
    updateState(HIGH_SCORES);
}

static int scoreIndex = 0;
void highScores(unsigned long elapsed) {
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

void play(uint interval) {
  game.update(interval);

  unsigned long now = millis();
  auto elapsed = now - getStateStart(); // update can change state

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
      highScores(elapsed);
      break;
    case GAME_START:
      gameStart(elapsed);
      break;
    case GAME_OVER:
      gameOver(elapsed);
      break;
    case GAME_WIN:
      gameEnd(elapsed);
      break;
    case PLAYING:
    case PLAYING_DEATH:
    case PLAYING_LEVEL_START:
    case PLAYING_LEVEL_END:
    case PLAYING_LOSE_LIFE:
      play(interval);
      break;
    default:
      Serial.println("ERROR: game state not handled.");
  }

  // TODO: display frame time every 5 seconds
  FastLED.show();
  prev = now;
}