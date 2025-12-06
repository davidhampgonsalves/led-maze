#include <Arduino.h>
#include <FastLED.h>

#include "esp_sleep.h"
#include "driver/rtc_io.h"
#include "FS.h"
#include "SD.h"

#include "ControlServer.h"
#include "Game.h"
#include "text.h"
#include "colors.h"
#include "state.h"
#include "animations.h"
#include "settings.h"
#include "file.h"

#define DEBUG false

#define BUTTON_PIN_BITMASK(GPIO) (1ULL << GPIO)
#define WAKEUP_GPIO              GPIO_NUM_33

#define NUM_LEDS 300
#define DATA_PIN 25
#define RELAY_PIN 27
#define uS_TO_S_FACTOR 1000000ULL
CRGB leds[NUM_LEDS];

ScoreList scores;
Level *titleLevel = NULL;
Level *deadLevel = NULL;
Level *winLevel = NULL;

uint8_t settingsBrightness;

void setup(){
  Serial.begin(115200);
  Serial.println("Starting up.");
  Serial.println(ESP.getFreeHeap());

  initSd();

  server.connect();
  Serial.println("Server is connected.");

  settingsBrightness = 30;

  FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);  // GRB ordering is assumed
  FastLED.setTemperature(0xFF7029);

  FastLED.clear();
  FastLED.show();

  // SD.begin(10);
  // File file = SD.open("/high-scores.txt", FILE_WRITE);
  // file.println("aaa:1");
  // file.println("bbb:2");
  // file.println("ccc:3");
  // file.close();

  // readHighScores();

  // writeHighScore("bbb", 100);
  // writeHighScore("eek", 1212);
  // writeHighScore("ccc", 50);
  // writeHighScore("Mee", 560);
  // writeHighScore("Loo", 750);
  // Serial.printf("is 200 a high score? %d\n", isHighScore(200));
  // Serial.printf("is 40 a high score? %d\n", isHighScore(40));

  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, HIGH);

  titleLevel = new Level("/screens/title.bmp");
  deadLevel = new Level("/screens/death.bmp");
  winLevel = new Level("/screens/win.bmp");
  Serial.println("title read complete.");

  Serial.println("Start up complete.");
  Serial.print("Total heap:");
  Serial.println(ESP.getHeapSize());
  Serial.print("Free heap: ");
  Serial.println(ESP.getFreeHeap());

  server.welcome();

  esp_sleep_enable_ext1_wakeup(BUTTON_PIN_BITMASK(WAKEUP_GPIO), ESP_EXT1_WAKEUP_ANY_HIGH);
  rtc_gpio_pulldown_en(WAKEUP_GPIO);
  rtc_gpio_pullup_dis(WAKEUP_GPIO);

  trackLastInput();
}

static uint32_t lastWS = 0;
static uint32_t deltaWS = 2000;
static unsigned long prev;

void gameStartInit() {
  server.stopSong();
}

void gameStart(unsigned long elapsed) {
  if(elapsed < WORD_WAIT) write("get", leds);
  else if(elapsed < WORD_WAIT * 2) write("ready", leds);
  else {
    game.start(1, false);
    setNextState(PLAYING_LEVEL_START);
  }
}

void gameEndInit() {
  server.stopSong();
  server.playSound("sounds/game-end.wav");
  server.win();
}

void gameEnd(unsigned long elapsed) {
  if(elapsed > 5000) {
    if(isHighScore(game.score))
      setNextState(HIGH_SCORE);
    else
      setNextState(TITLE);
  }

  winLevel->update(0);
  winLevel->draw(elapsed, leds);
}

void gameOver(unsigned long elapsed) {
  if(elapsed > 5000) {
    if(isHighScore(game.score))
      setNextState(HIGH_SCORE);
    else
      setNextState(TITLE);
  }

  deadLevel->update(0);
  deadLevel->draw(elapsed, leds);
}

void gameOverInit() {
  server.stopSong();
  server.playSound("sounds/game-over.wav");
  server.death();
}

void title(unsigned long elapsed) {
  if(elapsed < 5000) {
    titleLevel->update(0);
    titleLevel->draw(elapsed, leds);
  } else
    setNextState(RACING_ANIMATION);
}

void titleInit() {
  server.playSong("music/title.wav");
  server.welcome();
}


void highScore(unsigned long elapsed) {
    if(elapsed % (WORD_WAIT * 2) < WORD_WAIT)
      write("high", leds);
    else
      write("score", leds);
}

void highScoreInit() {
  server.playSong("music/highscore.wav");
  server.highScore(game.score);
}

void highScoresInit() {
  scores = readHighScores();
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
  if(elapsed < WORD_WAIT_LONG) write(scores.scores[scoreIndex].name, leds);
  else if(elapsed < WORD_WAIT_LONG * 2) write(scores.scores[scoreIndex].score, leds);
  else {
    scoreIndex += 1;
    if(scoreIndex >= MAX_HIGHSCORES) setNextState(TITLE);
  }
}

void loop() {
  FastLED.setBrightness(settingsBrightness);

  unsigned long now = millis();
  uint interval = now - prev;
  auto elapsed = now - getStateStart();

  if (now - lastWS >= deltaWS) {
    server.cleanupWsClients();
    lastWS = millis();
  }

  State state = curState();

  if(state != RACING_ANIMATION && state != RAINBOW_ANIMATION) FastLED.clear();

  if(isFreshState())
    switch(state)   {
      case TITLE:
        titleInit();
        break;
      case HIGH_SCORE:
        highScoreInit();
        break;
      case HIGH_SCORES:
        highScoresInit();
        break;
      case GAME_START:
        gameStartInit();
        break;
      case GAME_WIN:
        gameEndInit();
        break;
      case GAME_OVER:
        gameOverInit();
        break;
      case RACING_ANIMATION:
        racingAnimationInit(leds);
        break;
      case RAINBOW_ANIMATION:
        rainbowAnimationInit();
        break;
    }

  switch(state) {
    case START_UP:
      setNextState(TITLE);
      break;
    case TITLE:
      title(elapsed);
      break;
    case HIGH_SCORES:
      highScores(elapsed);
      break;
    case HIGH_SCORE:
      highScore(elapsed);
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
      game.update(interval);
      game.draw(leds, state, elapsed);
      break;
    case RACING_ANIMATION:
      racingAnimation(elapsed, leds);
      break;
    case RAINBOW_ANIMATION:
      rainbowAnimation(elapsed, leds);
      break;
    default:
      Serial.println("ERROR: game state not handled.");
  }

  transitionState();

  FastLED.show();
  prev = now;

  if((getLastInputTime() + 300000) < millis()) {
    Serial.flush();
    esp_sleep_enable_timer_wakeup(300 * uS_TO_S_FACTOR);
    esp_deep_sleep_start();
  }
  //Serial.printf("%lu - %lu\n", (getLastInputTime() + 30000000), millis());
}