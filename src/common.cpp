#include <Arduino.h>
#include <FastLED.h>
#include "pixeltypes.h"
#include <cmath>
#include "common.h"
#include "state.h"
#include "colors.h"

CRGB* getLed(int x, int y, CRGB leds[]) {
  if(x < 0 || y < 0 || x >= MAX_X || y >= MAX_Y) return NULL;
  if(x % 2 == 1) y = MAX_Y - y - 1;
  return &leds[x * MAX_Y + y];
}

void setLed(int x, int y, CRGB c, CRGB leds[]) {
  CRGB *px = getLed(x, y, leds);
  if(px == NULL) return;
  *px = c;
}

int indexToPos(int xy) {
  return (xy * PX_SIZE) + PX_CENTER; // return center
}

int posToIndex(unsigned long xy) {
  return round(xy / PX_SIZE);
}

int distanceBetween(int x1, int y1, int x2, int y2) {
    int dx = x2 - x1;
    int dy = y2 - y1;
    return abs(std::sqrt((dx * dx) + (dy * dy)));
}

CRGB colorAt(int x, int y, CRGB level[]) {
  if(x < 0 || y < 0 || x >= MAX_X || y >= MAX_Y) return HIDDEN_WALL_COLOR;

  return level[x * MAX_Y + y];
}

void setShimmerLed(unsigned long elapsed, int x, int y, CHSV hsv, CRGB leds[]) {
  const uint8_t min_brightness = hsv.v > 70 ? hsv.v - 70 : 0;
  const uint8_t max_brightness = hsv.v < 185 ? hsv.v + 70 : 255;

  uint8_t wave_input = (x * 4) + (y * 4) + (elapsed / 8);
  uint8_t sin_wave_value = sin8(wave_input);

  uint8_t brightness = map(sin_wave_value, 0, 255, min_brightness, max_brightness);

  CHSV shimmer_color = hsv;
  shimmer_color.v = brightness;

  setLed(x, y, hsv2rgb_spectrum(shimmer_color), leds);
}

void setFlameLed(int x, int y, CRGB leds[]) {
  if(random(10) > 8) return;
  uint8_t r  = random(180, 255);
  uint8_t g  = random(0, 100);
  setLed(x, y, CRGB{ r, g, 0 }, leds);
}

void dimDistanceLed(int x, int y, int distance, CRGB leds[]) {
  uint8_t dimming_factor = 256 - (256 * distance / 2500); // Calculate the scaling factor
  if(dimming_factor > 200) dimming_factor = 200;
  CRGB *px = getLed(x, y, leds);
  *px = px->fadeLightBy(dimming_factor);
}

void animateRing(unsigned long elapsed, CRGB c, State next, bool outward, int x, int y, CRGB leds[]) {
  setLed(x, y, BLACK, leds);

  if(elapsed > 1200) return updateState(next);

  if(outward)
    elapsed = 1200 - elapsed;

  if (elapsed >= 0 && elapsed < 300) {
    setLed(x + 2, y, c, leds);
    setLed(x - 2, y, c, leds);
    setLed(x, y + 2, c, leds);
    setLed(x, y - 2, c, leds);

    setLed(x + 1, y + 1, c, leds);
    setLed(x + 1, y - 1, c, leds);
    setLed(x - 1, y + 1, c, leds);
    setLed(x - 1, y - 1, c, leds);

    setLed(x + 1, y, BLACK, leds);
    setLed(x - 1, y, BLACK, leds);
    setLed(x, y + 1, BLACK, leds);
    setLed(x, y - 1, BLACK, leds);
  } else if (elapsed >= 400 && elapsed < 700) {
    setLed(x + 1, y, c, leds);
    setLed(x - 1, y, c, leds);
    setLed(x, y + 1, c, leds);
    setLed(x, y - 1, c, leds);
  } else if (elapsed >= 800 && elapsed <= 1100) {
    setLed(x, y, c, leds);
  }
}

double limitVelocity(double v, double limit) {
  if (v > limit)
    return limit;
  if (v < -limit)
    return -limit;

  return v;
}