#include "ESPAsyncWebServer.h"

class ControlServer {
  public:
    ControlServer();
    void cleanupWSClients();
    void connect();
  private:
    AsyncWebServer *server;
    AsyncWebSocket *ws;
};