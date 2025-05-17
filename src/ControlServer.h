#include "ESPAsyncWebServer.h"
#include "Game.h"

class ControlServer {
  public:
    ControlServer(Game*);
    void cleanupWSClients();
    void connect();
    void onEvent(AsyncWebSocket*, AsyncWebSocketClient*, AwsEventType , void*, uint8_t*, size_t);
  private:
    AsyncWebServer *server;
    AsyncWebSocket *ws;
    Game *game;
};