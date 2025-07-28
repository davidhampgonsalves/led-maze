#include <Arduino.h>
#include <cstring>
#include <FastLED.h>
#include <colors.h>
#include <common.h>

const unsigned long long letters[] = {
  0b011110110011110011111111110011110011,
  0b111110110011111110110011110011111110,
  0b011110110011110000110000110011011110,
  0b111110110011110011110011110011111110,
  0b111111110000111110110000110000111111,
  0b111111110000111110110000110000110000,
  0b011110110011110000110111110011011111,
  0b110011110011111111110011110011110011,
  0b011110001100001100001100001100011110,
  0b000111000011000011000011110011011110,
  0b110011110110111100111100110110110011,
  0b110000110000110000110000110000111111,
  0b110001111011111111110101110001110001,
  0b110001111001111101110111110011110001,
  0b011110110011110011110011110011011110,
  0b111110110011110011111100110000110000,
  0b011110110011110011110111110010011101,
  0b111110110011110011111110110011110011,
  0b011110110000011110000011110011011110,
  0b111111001100001100001100001100001100,
  0b110011110011110011110011110011011110,
  0b110011110011110011110011011110001100,
  0b110001110001110101111111111011110001,
  0b110011110011100001011110110011110011,
  0b110011110011110011011110001100001100,
  0b111111000111001110011100111000111111,
};
const unsigned long long digits[] = {
  0b011110110011110111111011110011011110,
  0b001100011100001100001100001100011110,
  0b011110110011000011011110110000111111,
  0b011110000011001110000011110011011110,
  0b000110001110011110110110111111000110,
  0b111110110000111110000011110011011110,
  0b011110110000111110110011110011011110,
  0b111111110011000011000110001100001100,
  0b011110110011011110110011110011011110,
  0b011110110011110011011111000011011110,
};
const unsigned int FONT_SIZE=6;
const unsigned int CHAR_OFFSET=4;

void writeLetter(int charCode, int offset, CRGB leds[], const unsigned long long * font) {
  for(int i=0 ; i < FONT_SIZE * FONT_SIZE ; i++) {
    bool bit = (font[charCode] >> i) & 1;
    int x = FONT_SIZE - (i % FONT_SIZE) - 1;
    int y = (i / FONT_SIZE) + offset;

    if(bit) setLed(x, y, TEXT_COLOR, leds);
  }
}

void write(const char * text, CRGB leds[]) {
  int len = strlen(text);
  int letterMargin = len >= 5 ? 2 : CHAR_OFFSET;
  int textHeight = len * (FONT_SIZE + letterMargin);
  int offset = (50 - textHeight) / 2;
  if(textHeight > 50) {
    offset = 0;
  }

  for (int i=len-1; i >= 0 ; i--) {
    int charCode = static_cast<int>(text[i]);

    if(charCode >= 97 && charCode <= 122)
      writeLetter(charCode - 97, offset, leds, letters);
    else if(charCode >= 48 && charCode <= 57)
      writeLetter(charCode - 48, offset, leds, digits);
    else if(charCode == 32) {} // space
    else
      Serial.printf("Do not have character def for %d\n", charCode);

    offset += FONT_SIZE + letterMargin;
  }
}

void writeFixed5(long num, CRGB leds[]) {
  char buff [6];
  sprintf(buff, "%05d", num);
  write(buff, leds);
}

void write(std::string text, CRGB leds[]) {
  write(text.c_str(), leds);
}

void write(long num, CRGB leds[]) {
  std::string str = std::to_string(num);
  write(str.c_str(), leds);
}

void writeTitle(const char * text, CRGB leds[], unsigned long start) {}