extern int MAX_X;
extern int MAX_Y;

void setLed(int x, int y, CHSV c, CRGB leds[]) {
  if(x % 2 == 1) y = MAX_Y - y - 1;

  c.value = dim8_lin(c.value); // gamma correction

  // Serial.print("setLed for ");
  // Serial.print(x);
  // Serial.print(" is ");
  // Serial.println(x * MAX_Y + y);
  leds[x * MAX_Y + y] = c;
}