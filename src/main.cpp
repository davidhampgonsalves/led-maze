#include <Arduino.h>
#include <FastLED.h>

#include "ControlServer.h"
#include "Game.h"

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

void loop() {
  // server->cleanupWSClients();

  // game.update();

  // FastLED.clear();

  game.draw(leds);
  // level->draw();

  FastLED.show();

  // delay(30); // TODO: maintain FPS
}