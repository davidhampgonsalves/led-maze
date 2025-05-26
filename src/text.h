#include <cstring>
#include <FastLED.h>
#include <colors.h>
#include <common.h>

const int WORD_WAIT = 900;
const char letters[] = {
  0b0000001101010111,
  0b0100011101010111,
  0b0000011101000111,
  0b0001011101010111,
  0b0000011101010110,
  0b0011010001100100,
  0b0111010101110011,
  0b0100011101010101,
  0b0010000000100010,
  0b0010000000100110,
  0b0100010101100101,
  0b0010001000100010,
  0b0000011101110101,
  0b0000011101010101,
  0b0000011101010111,
  0b0111010101111000,
  0b0111010101110001,
  0b0000011101010100,
  0b0000001100100110,
  0b0100111001000100,
  0b0000010101010111,
  0b0000010101010011,
  0b0000010101110111,
  0b0000010100100101,
  0b0101010101110011,
  0b0000011000100011,
};
const char digits[] = {
  0b0111010101010111,
  0b0110001000100111,
  0b0111000101100111,
  0b0111000100110111,
  0b0101010101110001,
  0b0111010000110111,
  0b0111010001110111,
  0b0111000100100100,
  0b0111010101110111,
  0b0111011100010111,
};
void write(char * text, CRGB leds[]) {
  int len = strlen(text);
  int offset = 50 - (len * 5) / 2;
  for (int i=0; i < len ; i++) {
    int charCode = static_cast<int>(text[i]);

    if(charCode > 47 && charCode < 58)
      writeLetter(charCode - 47, offset, leds, digits);
    else if(charCode > 96 && charCode < 123)
      writeLetter(charCode - 96, offset, leds, letters);
    else if(charCode == 32) {} // space
    else
      Serial.printf("Do not have character def for %d\n", charCode);

    offset += 5
  }
}

void writeLetter(int charCode, int offset, CRGB leds[], const char * font) {
  for(int i=0 ; i < 16 ; i++) {
    unsigned int bit = font[charCode] & 1;
    int x = i % 4;
    int y = (i / 4) + offset;

    if(bit == 1) setLed(x, y, CHSV(200, 200, 150), leds);

    bit >>= 1;
  }
}