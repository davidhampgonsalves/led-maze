#include "ESPAsyncWebServer.h"

class ControlServer {
  public:
    ControlServer();
    void cleanupWsClients();
    void connect();
};