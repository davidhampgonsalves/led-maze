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

const char *longString = R""""(
<html>
    <head>
      <style>
        body {
          display: flex;
          flex-direction: column;
          height: 100%;
        }

        #start {
          padding: 1em;
          : 1em;
          height: 10em;
          margin: 1em;
          font-size: 2em;
        }

        #position {
          background: black;
          color: green;
          width: fit-content;
          padding: 0.5em;
          margin: 1em;
        }

        #console {
          background: black;
          color: green;
          padding: 1em;
          max-height: 50%;
          font-size: 0.8em;
          font-family: monospace;
          overflow-y: auto;
        }
      </style>
      <meta name="viewport" content="width=device-width, initial-scale=1.0" />
    </head>
    <body>
      <button id="start">Start</button>

      <div id="position">...</div>

      <div id="console">
        # Console
      </div>

      <script type="module">
        async function ensureDevicePermissions() {
          if (typeof DeviceOrientationEvent.requestPermission !== "function") return;

          await DeviceOrientationEvent.requestPermission().then((state) => {
            if (state === "granted") return;

            console.error("You must grant permission for sensor data to play");
          });
        };


        const gateway = `wss://maze.davidhampgonsalves.com:9000/ws`;
        let websocket;
        function initWebSocket() {
          console.log('Trying to open a WebSocket connection...');
          websocket = new WebSocket(gateway);
          websocket.onopen    = onOpen;
          websocket.onclose   = onClose;
          websocket.onmessage = onMessage;
        }
        function onOpen(event) {
          console.log('Connection opened');
        }
        function onClose(event) {
          console.log('Connection closed');
          setTimeout(initWebSocket, 500);
        }
        function onMessage(event) {
          console.log("onMessage:", event.data)
        }






        const consoleEl = document.querySelector("#console");
        function log(...args) {
          const el = document.createElement('div');
          el.innerText = args.join(", ");
          consoleEl.appendChild(el);
        }
        console.log = log
        console.error = log

        const positionEl = document.querySelector("#position");
        const formatNumber = (n) => String(Math.round(n)).padStart(3, '0');
        let lastSendTime = 0;
        function handleOrientationEvent(event) {
          positionEl.innerHTML = formatNumber(event.beta) + " / " + formatNumber(event.gamma);

          if(event.beta == null) {
            console.log("beta not set");
            return;
          }

          const now = Date.now();
          if (now - lastSendTime >= 300) {
            lastSendTime = now;
            console.log("sending");
            websocket.send(event.beta.toFixed(2) + "," + event.gamma.toFixed(2));
          }
        }

        async function start(e) {
          await ensureDevicePermissions();
          document.body.removeChild(e.target);

          window.addEventListener('deviceorientation', handleOrientationEvent);

          initWebSocket();
        }

        document.querySelector("#start").addEventListener("click", start);
      </script>
    </body>
    </html>
    <script>
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
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    // request->send(SPIFFS, "/index.html", String(), false);
    request->send(200, "text/html", longString);
  });

  // wsHandler.onMessage([](AsyncWebSocket *server, AsyncWebSocketClient *client, const uint8_t *data, size_t len) {
  //   Serial.print("onmessage!!");
  //   Serial.printf("Client %" PRIu32 " data: %s\n", client->id(), (const char *)data);

  //   game.updateAccel(1, 1); // TODO
  // });

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
    std::string coords((const char *)data);
    int pos = coords.find(',');
    double beta = std::stod(coords.substr(0, pos));
    double gamma = std::stod(coords.substr(pos + 1));

    Serial.printf("Client %" PRIu32 " data: %f, %f\n", client->id(), beta, gamma);
    game.updateAccel(beta, gamma);
  });

  wsHandler.onFragment([](AsyncWebSocket *server, AsyncWebSocketClient *client, const AwsFrameInfo *frameInfo, const uint8_t *data, size_t len) {
    Serial.printf("Client %" PRIu32 " fragment %" PRIu32 ": %s\n", client->id(), frameInfo->num, (const char *)data);
  });

  server.addHandler(&ws);
  server.begin();
}
