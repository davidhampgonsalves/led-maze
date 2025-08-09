#include "ESPAsyncWebServer.h"

class ControlServer {
  public:
    ControlServer();
    void cleanupWsClients();
    void connect();
    void playSong(const char* path);
    void playSound(const char* path);
    void stopSong();
    void highScore(int score);
  private:
    char indexData[7000];
};

extern ControlServer server;