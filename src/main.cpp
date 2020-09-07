#include <WiFi.h>
#include <SPIFFS.h>
#include <ESPAsyncWebServer.h>
#include <WebSocketsServer.h>
/*You will need to provide the next file with the following content:

const char *ssid = "YOUR_SSID";
const char *password = "YOUR_PASSWORD";
*/
#include "PrivateConfigParameters.h"

const int http_port = 80;
const int ws_port = 1337;

AsyncWebServer server(http_port);
WebSocketsServer ws = WebSocketsServer(ws_port);
char msg_buf[10];
uint8_t barValue = 0;
bool animate = false;
unsigned long lastAnimStep;
uint8_t stepInterval;

void startAnimation();
void stopAnimation();

// Callback: receiving any WebSocket message
void onWebSocketEvent(uint8_t client_num, WStype_t type, uint8_t *payload, size_t length)
{
  switch (type)
  {
  // Client disconnected
  case WStype_DISCONNECTED:
    Serial.printf("[%u] Disconnected!\n", client_num);
    break;
  // New client connected
  case WStype_CONNECTED:
  {
    IPAddress ip = ws.remoteIP(client_num);
    Serial.printf("[%u] Connection from ", client_num);
    Serial.println(ip.toString());
  }
  break;
  // Handle text messages from client
  case WStype_TEXT:
    Serial.printf("[%u] Received text: %s\n", client_num, payload);
    if (strcmp((char *)payload, "clearBarValue") == 0)
    {
      barValue = 0;
      stopAnimation();
    }
    else if (strcmp((char *)payload, "startBarAnimation") == 0)
    {
      Serial.printf("Sending to [%u]: %s\n", client_num, msg_buf);
      startAnimation();
    }
    // else if ( atoi((char *)payload) >= 0 && atoi((char *)payload) <= 100 ){
    //   //sprintf(msg_buf, "%d", barValue);
    //   Serial.printf("Sending to [%u]: %s\n", client_num, msg_buf);
    //   barValue = atoi((char *)payload);
    // }
    else
    {
      Serial.println("[%u] Message not recognized");
    }
    break;
  case WStype_BIN:
  case WStype_ERROR:
  case WStype_FRAGMENT_TEXT_START:
  case WStype_FRAGMENT_BIN_START:
  case WStype_FRAGMENT:
  case WStype_FRAGMENT_FIN:
  default:
    break;
  }
}
void startAnimation()
{
  animate = true;
}

void stopAnimation()
{
  animate = false;
}

// Callback: send homepage
void onIndexRequest(AsyncWebServerRequest *request)
{
  IPAddress remote_ip = request->client()->remoteIP();
  Serial.println("[" + remote_ip.toString() +
                 "] HTTP GET request of " + request->url());
  request->send(SPIFFS, "/index.html", "text/html");
}

void onPageNotFound(AsyncWebServerRequest *request)
{
  IPAddress remote_ip = request->client()->remoteIP();
  Serial.println("[" + remote_ip.toString() +
                 "] HTTP GET request of " + request->url());
  request->send(404, "text/plain", "Not found");
}

void setup()
{
  Serial.begin(115200);

  if (!SPIFFS.begin())
  {
    Serial.println("Error mounting SPIFFS");
    while (1)
      ;
  }

  //WiFi.softAP(ssid, password);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  //Serial.println("AP running");
  Serial.print("My IP address: ");
  //Serial.println(WiFi.softAPIP());

  Serial.println(WiFi.localIP());

  server.on("/", HTTP_GET, onIndexRequest);
  server.onNotFound(onPageNotFound);

  server.begin();

  ws.begin();
  ws.onEvent(onWebSocketEvent);

  stepInterval = 500;
}

void loop()
{
 // WiFiClient client = server.available();

  if (animate && ( millis() - lastAnimStep >= stepInterval))
  {
    lastAnimStep = millis();
    barValue += 1;
    if (barValue > 100){
      barValue = 100;
      stopAnimation();
    }

    char barValueString[4];
    sprintf(barValueString, "%d", barValue);
    ws.broadcastTXT(barValueString);
  }
  // Look for and handle WebSocket data
  ws.loop();
}