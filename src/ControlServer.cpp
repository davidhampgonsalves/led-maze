#include <AsyncTCP.h>
#include "ESPAsyncWebServer.h"
#include "SPIFFS.h"
#include "WiFi.h"
#include <ESPmDNS.h>
#include <iostream>
#include <string>
#include <sstream>
#include "ControlServer.h"
#include "state.h"
#include "Game.h"
#include "file.h"

// Replace with your network credentials
const char *ssid = "orange";
const char *password = "isacolor";

static AsyncWebServer server(80);
static AsyncWebSocketMessageHandler wsHandler;
static AsyncWebSocket ws("/ws", wsHandler.eventHandler());

void ControlServer::cleanupWsClients() {
  // TODO: call this at most once per second
  ws.cleanupClients();
}


ControlServer::ControlServer() { }

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
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    // request->send(SPIFFS, "/index.html", String(), false);
    request->send(200, "text/html", readFile("/index.html").c_str());
  });

  wsHandler.onConnect([](AsyncWebSocket *server, AsyncWebSocketClient *client) {
    Serial.printf("Client %" PRIu32 " connected\n", client->id());
    server->textAll("New client: " + String(client->id()));

    updateState(GAME_START);
  });

  wsHandler.onDisconnect([](AsyncWebSocket *server, uint32_t clientId) {
    Serial.printf("Client %" PRIu32 " disconnected\n", clientId);
    server->textAll("Client " + String(clientId) + " disconnected");

    updateState(TITLE);
  });

  wsHandler.onError([](AsyncWebSocket *server, AsyncWebSocketClient *client, uint16_t errorCode, const char *reason, size_t len) {
    Serial.printf("Client %" PRIu32 " error: %" PRIu16 ": %s\n", client->id(), errorCode, reason);
  });

  wsHandler.onMessage([](AsyncWebSocket *server, AsyncWebSocketClient *client, const uint8_t *data, size_t len) {
    std::string msg((const char *)data);
    int typePos = msg.find(':');
    std::string type = msg.substr(0, typePos);

    if(type == "pos") {
      int pos = msg.find(',');
      double beta = std::stod(msg.substr(typePos + 1, pos));
      double gamma = std::stod(msg.substr(pos + 1));
      game.updateAccel(beta, gamma);
      // Serial.printf("Client %" PRIu32 " data: %f, %f\n", client->id(), beta, gamma);
    } else if(type == "gravity")
      game.GRAVITY = std::stod(msg.substr(typePos + 1));
    else if(type == "accel")
      game.ACCEL_DIVISOR = std::stoi(msg.substr(typePos + 1));
    else if(type == "friction") {
      game.FRICTION = std::stod(msg.substr(typePos + 1));
      Serial.printf("friction: %f", game.FRICTION);
    } else
      Serial.printf("ERROR: %s msg type not handled.\n");
  });

  wsHandler.onFragment([](AsyncWebSocket *server, AsyncWebSocketClient *client, const AwsFrameInfo *frameInfo, const uint8_t *data, size_t len) {
    Serial.printf("Client %" PRIu32 " fragment %" PRIu32 ": %s\n", client->id(), frameInfo->num, (const char *)data);
  });

  server.addHandler(&ws);
  server.begin();
}
