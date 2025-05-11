#include "ESPAsyncWebServer.h"
#include "SPIFFS.h"
#include "WiFi.h"
#include <ESPmDNS.h>
#include "ControlServer.h"

// Replace with your network credentials
const char *ssid = "orange";
const char *password = "isacolor";

void ControlServer::cleanupWSClients() {
  // TODO: call this at most once per second
  ws->cleanupClients();
}

ControlServer::ControlServer() {
  server = new AsyncWebServer(80);
  ws = new AsyncWebSocket("/ws");
}

void handleWebSocketMessage(void *arg, uint8_t *data, size_t len) {
  AwsFrameInfo *info = (AwsFrameInfo *)arg;
  if (info->final && info->index == 0 && info->len == len &&
      info->opcode == WS_TEXT) {
    data[len] = 0;
    Serial.printf((char *)data);
    // if (strcmp((char *)data, "toggle") == 0) {
    //   // notifyClients();
    // }
  }
}

void onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client,
             AwsEventType type, void *arg, uint8_t *data, size_t len) {
  switch (type) {
  case WS_EVT_CONNECT:
    Serial.printf("WebSocket client #%u connected from %s\n", client->id(),
                  client->remoteIP().toString().c_str());
    break;
  case WS_EVT_DISCONNECT:
    Serial.printf("WebSocket client #%u disconnected\n", client->id());
    break;
  case WS_EVT_DATA:
    handleWebSocketMessage(arg, data, len);
    break;
  case WS_EVT_PONG:
  case WS_EVT_ERROR:
    break;
  }
}

const char *longString = R""""(
  <html>
    <body>
      Welcome to maze
      <script>
var gateway = `ws://${window.location.hostname}/ws`;
var websocket;
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
  setTimeout(initWebSocket, 2000);
}
function onMessage(event) {
  console.log(event);
}

function handleOrientation(event) {
  const absolute = event.absolute;
  const alpha = event.alpha;
  const beta = event.beta;
  const gamma = event.gamma;

  console.log(absolute, alpha, beta, gamma);
  websocket.send({ absolute, alpha, beta, gamma })
}

// need to call after user interaction
if (typeof DeviceOrientationEvent.requestPermission === 'function' && typeof DeviceMotionEvent.requestPermission === 'function') {

DeviceOrientationEvent.requestPermission()

window.addEventListener("deviceorientation", handleOrientation, true);
initWebSocket();
      </script>
    </body>
  </html>
)"""";

void ControlServer::connect() {
  // Connect to Wi-Fi
  IPAddress local_IP(192, 168, 1, 184);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }

  // if (!MDNS.begin("maze")) {
  //   Serial.println("Error setting up MDNS responder!");
  // }
  // Serial.println("mDNS responder started");

  // Print ESP32 Local IP Address
  Serial.println(WiFi.localIP());

  // Route for root / web page
  server->on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    // request->send(SPIFFS, "/index.html", String(), false);
    request->send(200, "text/html", longString);
  });

  ws->onEvent(onEvent);
  server->addHandler(ws);

  server->begin();
}
