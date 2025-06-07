#include <cstring>
#include <FastLED.h>
#include <colors.h>
#include <common.h>

const uint32_t letters[] = {
  0b0111010001111111000110001,
  0b1111010001111101000111110,
  0b0111110000100001000001111,
  0b1111010001100011000111110,
  0b1111110000111101000011111,
  0b1111110000111101000010000,
  0b0111110000100111000101111,
  0b1000110001111111000110001,
  0b0111000100001000010001110,
  0b0000100001000011000101110,
  0b1001010100110001010010010,
  0b0100001000010000100001111,
  0b1000111011101011000110001,
  0b1000111001101011001110001,
  0b0111010001100011000101110,
  0b1111010001111101000010000,
  0b0111010001100010111000011,
  0b1111010001111101010010011,
  0b0111110000011100000111110,
  0b1111100100001000010000100,
  0b1000110001100011000101110,
  0b1000110001100010101000100,
  0b1000110001101011101110001,
  0b1000101010001000101010001,
  0b1000110001010100010000100,
  0b1111100010001000100011111
};
const uint32_t digits[] = {
  0b0111010001100011000101110,
  0b0010011100001000010011111,
  0b1111000001011101000011111,
  0b1111000001011100000111110,
  0b0011001010100101111100010,
  0b1111010000111100000111110,
  0b0111010000111101000101110,
  0b1111100001000100010001000,
  0b0111010001011101000101110,
  0b0111010001011110000111110,
};
const uint FONT_SIZE=5;
const uint CHAR_OFFSET=2;

void printBits(size_t const size, uint32_t b)
{
    unsigned char byte;
    int i, j;

    for (j = size; j >= 0; j--) {
        byte = (b >> j) & 1;
        Serial.printf("%u", byte);
    }
    Serial.println("");
}

void writeLetter(int charCode, int offset, CRGB leds[], const uint32_t * font) {
  for(int i=0 ; i < FONT_SIZE * FONT_SIZE ; i++) {
    bool bit = (font[charCode] >> i) & 1;
    int x = FONT_SIZE - (i % FONT_SIZE);
    int y = (i / FONT_SIZE) + offset;

    if(bit) setLed(x, y, TEXT_COLOR, leds);
  }
}

void write(const char * text, CRGB leds[]) {
  int len = strlen(text);
  int textHeight = len * (FONT_SIZE + CHAR_OFFSET);
  int offset = (50 - textHeight) / 2;
  if(textHeight > 50) {
    Serial.printf("ERROR: '%s' is too long\n", text);
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

    offset += FONT_SIZE + CHAR_OFFSET;
  }
}

void write(std::string text, CRGB leds[]) {
  write(text.c_str(), leds);
}

void write(long num, CRGB leds[]) {
  std::string str = std::to_string(num);
  write(str.c_str(), leds);
}

void writeTitle(const char * text, CRGB leds[], unsigned long start) {}