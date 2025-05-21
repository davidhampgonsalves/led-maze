#include "ESPAsyncWebServer.h"
#include "SPIFFS.h"
#include "WiFi.h"
#include <ESPmDNS.h>
#include "ControlServer.h"
#include "Game.h"

// Replace with your network credentials
const char *ssid = "orange";
const char *password = "isacolor";

void ControlServer::cleanupWSClients() {
  // TODO: call this at most once per second
  ws->cleanupClients();
}

ControlServer::ControlServer() {
  AsyncWebSocketMessageHandler wsHandler;
  server = new AsyncWebServer(80);
  ws = new AsyncWebSocket("/ws", wsHandler.eventHandler());
}

// void onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client,
//              AwsEventType type, void *arg, uint8_t *data, size_t len) {
//   switch (type) {
//   case WS_EVT_CONNECT:
//     Serial.printf("WebSocket client #%u connected from %s\n", client->id(),
//                   client->remoteIP().toString().c_str());
//     break;
//   case WS_EVT_DISCONNECT:
//     Serial.printf("WebSocket client #%u disconnected\n", client->id());
//     break;
//   case WS_EVT_DATA: {
//     AwsFrameInfo *info = (AwsFrameInfo *)arg;
//     if (info->final && info->index == 0 && info->len == len &&
//         info->opcode == WS_TEXT) {
//       data[len] = 0;
//       Serial.printf((char *)data);

//       game->updateAccel(1, 1); // TODO
//     }
//     break;
//   } case WS_EVT_PONG:
//   case WS_EVT_ERROR:
//     break;
//   }
// }

const char *longString = R""""(
  page
)"""";

void ControlServer::connect() {
  // Connect to Wi-Fi
  IPAddress local_IP(192, 168, 1, 184);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }

  // Print ESP32 Local IP Address
  Serial.println(WiFi.localIP());

  // Route for root / web page
  server->on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    // request->send(SPIFFS, "/index.html", String(), false);
    request->send(200, "text/html", longString);
  });

  // ws->onEvent(onEvent);
  // server->addHandler(ws);

  Serial.println("setting up web sockets");
  // ERROR
  // wsHandler->onMessage([](AsyncWebSocket *server, AsyncWebSocketClient *client, const uint8_t *data, size_t len) {
  //   // Serial.printf("Client %" PRIu32 " data: %s\n", client->id(), (const char *)data);

  //   // game.updateAccel(1, 1); // TODO
  // });
  Serial.println("done");

  server->begin();
}
