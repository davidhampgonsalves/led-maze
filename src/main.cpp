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
  // FastLED.setBrightness(80);
}

int i = 2;
int increment = 1;
void loop() {
  server->cleanupWSClients();

  game.update();
  game.draw(leds);
  // level->draw();

  FastLED.clear();
  FastLED.show();

  // Turn the LED on, then pause
  leds[i].setRGB(255, 0, 0);
  leds[i-1].setRGB(0, 255, 0);
  leds[i-2].setRGB(0, 0, 255);
  FastLED.show();
  delay(40);
  i += increment;

  if(i >= NUM_LEDS || i-2 <= 0) increment = increment * -1;
}