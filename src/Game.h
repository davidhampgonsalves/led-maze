class Game{
  public:
    Game();
    void updateAccel(double, double);
    void update();
    void draw(CRGB leds[]);
  private:
    int accelX;
    int accelY;
    int posX;
    int posY;

    int x();
    int y();
    unsigned long time;
};