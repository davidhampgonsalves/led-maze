#include "ESPAsyncWebServer.h"

class ControlServer {
  public:
    ControlServer();
    void cleanupWSClients();
    void connect();
    void onEvent(AsyncWebSocket*, AsyncWebSocketClient*, AwsEventType , void*, uint8_t*, size_t);
  private:
    AsyncWebServer *server;
    AsyncWebSocket *ws;
    AsyncWebSocketMessageHandler *wsHandler;
};