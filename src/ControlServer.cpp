#include <AsyncTCP.h>
#include "ESPAsyncWebServer.h"
#include "SPIFFS.h"
#include "WiFi.h"
#include <ESPmDNS.h>
#include <iostream>
#include <string>
#include <sstream>
#include "ControlServer.h"
#include "settings.h"
#include "state.h"
#include "Game.h"
#include "file.h"
#include <SD.h>

ControlServer server;

// Replace with your network credentials
const char *ssid = "orange";
const char *password = "isacolor";

static AsyncWebServer webServer(80);
static AsyncWebSocketMessageHandler wsHandler;
static AsyncWebSocket ws("/ws", wsHandler.eventHandler());

static char msgBuff[128];

void ControlServer::cleanupWsClients() {
  // TODO: call this at most once per second
  ws.cleanupClients();
}

ControlServer::ControlServer() {
}

void ControlServer::playSong(const char* path) {
  // const size_t len = measureJson(doc);
  // AsyncWebSocketMessageBuffer* buffer = _ws->makeBuffer(len);
  // serializeJson(doc, buffer->get(), len);
  // ws->textAll(buffer);

  ws.printfAll("{\"type\":\"PLAY_SONG\",\"url\":\"%s\"}", path);
}

void ControlServer::playSound(const char* path) {
  // const size_t len = measureJson(doc);
  // AsyncWebSocketMessageBuffer* buffer = _ws->makeBuffer(len);
  // serializeJson(doc, buffer->get(), len);
  // ws->textAll(buffer);

  ws.printfAll("{\"type\":\"PLAY_SOUND\",\"url\":\"%s\"}", path);
}

void ControlServer::stopSong() {
  ws.printfAll("{\"type\":\"STOP_SONG\"}");
}

void ControlServer::highScore(int score) {
  ws.printfAll("{\"type\":\"HIGH_SCORE\",\"score\":\"%d\"}", score);
}

void ControlServer::welcome() {
  ws.printfAll("{\"type\":\"WELCOME\"}");
}

void ControlServer::death() {
  ws.printfAll("{\"type\":\"DEATH\"}");
}

void ControlServer::win() {
  ws.printfAll("{\"type\":\"WIN\"}");
}

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

  // readFile("/index.html", indexData);

  // Route for root / web page
  webServer.on("/", HTTP_GET, [this](AsyncWebServerRequest *request) {
    // request->send(200, "text/html", indexData);
    if(!SD.begin(5)){
      Serial.println("Card Mount Failed");
      return;
    }
    request->send(SD, "/index.html", "text/html; charset=utf-8");
  });

  wsHandler.onConnect([](AsyncWebSocket *server, AsyncWebSocketClient *client) {
    Serial.printf("Client %" PRIu32 " connected\n", client->id());
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
    strcpy(msgBuff, (const char *)data);

    char* type = strtok(msgBuff, ":");
    Serial.printf("type: %s\n", type);

    // std::string msg((const char *)data);
    // int typePos = msg.find(':');
    // std::string type = msg.substr(0, typePos);


    if(strcmp(type, "start") == 0) {
      updateState(GAME_START);
    } else if(strcmp(type, "pos") == 0) {
      double beta = std::stod(strtok(NULL, ","));
      double gamma = std::stod(strtok(NULL, ","));

      Serial.printf("data: %s %f, %f\n", type, beta, gamma);
      game.updateAccel(beta, gamma);
    } else if(strcmp(type, "highscore") == 0) {
      long score = std::stol(strtok(NULL, ","));
      char* name = strtok(NULL, ",");

      Serial.printf("%s %ld\n", name, score);
      writeHighScore(name, score);
      updateState(HIGH_SCORES);
    } else if(strcmp(type, "brightness") == 0)
      settingsBrightness = std::stod(strtok(NULL, ","));
    else
      Serial.printf("ERROR: %s msg type not handled.\n");
  });

  wsHandler.onFragment([](AsyncWebSocket *server, AsyncWebSocketClient *client, const AwsFrameInfo *frameInfo, const uint8_t *data, size_t len) {
    Serial.printf("Client %" PRIu32 " fragment %" PRIu32 ": %s\n", client->id(), frameInfo->num, (const char *)data);
  });

  webServer.addHandler(&ws);
  webServer.begin();
}
