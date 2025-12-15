#include <Arduino.h>
#include <FastLED.h>

#include "esp_sleep.h"
#include "driver/rtc_io.h"
#include "FS.h"
#include "SD.h"
#include <SPI.h>

#include "ControlServer.h"
#include "Game.h"
#include "text.h"
#include "colors.h"
#include "state.h"
#include "animations.h"
#include "settings.h"
#include "file.h"
#include "screen.h"

#define DEBUG false

#define BUTTON_PIN_BITMASK(GPIO) (1ULL << GPIO)
#define WAKEUP_GPIO              GPIO_NUM_33

#define NUM_LEDS 300
#define DATA_PIN 25
#define RELAY_PIN 27
#define uS_TO_S_FACTOR 1000000ULL
CRGB leds[NUM_LEDS];

Screen *titleScreen = NULL;
Screen *deadScreen = NULL;
Screen *winScreen = NULL;

uint8_t settingsBrightness;
unsigned long startTime;
bool goingToSleep;

void setup() {
  Serial.begin(115200);
  Serial.println("Starting up.");
  Serial.println(ESP.getFreeHeap());

  server.connect();
  Serial.println("Server is connected.");

  settingsBrightness = 30;

  FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);  // GRB ordering is assumed
  FastLED.setTemperature(0xFF7029);

  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, HIGH);

  SPI.begin();
  SPI.setClockDivider(SPI_CLOCK_DIV128);

  while(!SD.begin(5)) {
    Serial.println("Card Mount Failed");
    setLed(0, 0, FIRE_RGB, leds);
    FastLED.show();
  }

  FastLED.clear();
  FastLED.show();

  titleScreen = new Screen("/screens/title.bmp");
  deadScreen = new Screen("/screens/death.bmp");
  winScreen = new Screen("/screens/win.bmp");
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

  pinMode(WAKEUP_GPIO, INPUT);

  trackLastInput();

  startTime = millis();

  // RESET HIGH SCORES
  // File file = SD.open("/high-scores.txt", FILE_WRITE);
  // file.println("one,100");
  // file.println("two,75");
  // file.println("tri,50");
  // file.close();

  readHighScores();

  goingToSleep = false;

  resetState();
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

void gameEnd(unsigned long elapsed) {
  if(elapsed > 5000) {
    if(isHighScore(game.score))
      setNextState(HIGH_SCORE);
    else
      setNextState(TITLE);
  }

  winScreen->draw(elapsed, leds);
}

void gameOverInit() {
  server.stopSong();
  server.playSound("sounds/game-over.wav");
}

void gameOver(unsigned long elapsed) {
  if(elapsed > 5000) {
    if(isHighScore(game.score))
      setNextState(HIGH_SCORE);
    else
      setNextState(TITLE);
  }

  deadScreen->draw(elapsed, leds);
}

void title(unsigned long elapsed) {
  if(elapsed < 5000)
    titleScreen->draw(elapsed, leds);
  else
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

void highScores(unsigned long elapsed) {
  if(elapsed < WORD_WAIT * 3) {
    if(elapsed < WORD_WAIT) {
      write("hall", leds);
    } else if(elapsed < WORD_WAIT * 2)
      write("of", leds);
    else
      write("fame", leds);

    return;
  }

  elapsed -= WORD_WAIT * 3;

  if(elapsed < WORD_WAIT_LONG) write(hsName1, leds);
  else if(elapsed < WORD_WAIT_LONG * 2) write(highScore1, leds);
  else if(elapsed < WORD_WAIT_LONG * 3) write(hsName2, leds);
  else if(elapsed < WORD_WAIT_LONG * 4) write(highScore2, leds);
  else if(elapsed < WORD_WAIT_LONG * 5) write(hsName3, leds);
  else if(elapsed < WORD_WAIT_LONG * 6) write(highScore3, leds);
  else setNextState(TITLE);
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
      case GAME_START:
        gameStartInit();
        break;
      case GAME_OVER:
        gameOverInit();
        break;
      case RACING_ANIMATION:
        racingAnimationInit(leds);
        break;
      case RAINBOW_ANIMATION:
        server.stopSong();
        server.playSound("sounds/game-end.wav");

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

  int buttonState = digitalRead(WAKEUP_GPIO);
  // need to wait for button press to complete before we sleep or else still pressed button will
  // trigger an imediate wake up.
  if(startTime - now > 1000 && buttonState == HIGH) goingToSleep = true;

  if((getLastInputTime() + 300000) < now || (goingToSleep && buttonState == LOW)) {
    Serial.flush();
    int sleepDur = random(60*60, 6*60*60);
    esp_sleep_enable_timer_wakeup(sleepDur * uS_TO_S_FACTOR);
    esp_deep_sleep_start();
  }
}