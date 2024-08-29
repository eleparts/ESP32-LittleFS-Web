/*
*
* ESP32로 웹 서버를 실행, 웹 소켓을 이용해 버튼으로 데이터를 받아 LED를 ON/OFF 해 줍니다.
* LittleFS을 사용해 ESP32 내부 공간에 별도의 HTML 및 JS 파일을 저장하고 이 파일로 웹 페이지를 실행해 줍니다.
* 
*/
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <LittleFS.h>

#define LED     2

/* 사용할 WIFI 접속 정보 입력 */
const char *ssid     = "WIFI_SSID"; 
const char *password = "WIFI_PASSWORD";

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);
AsyncWebSocket ws("/ws");

// Initialize LittleFS
void initLittleFS() {
  if (!LittleFS.begin())
  {
  Serial.println("An error has occurred while mounting LittleFS");
  }
  Serial.println("LittleFS mounted successfully");
}

// Initialize WiFi
void initWiFi() {
  
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi..");

  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println();

  // Print ESP32 IP Address
  Serial.println(WiFi.localIP());
}

// TEMPLATE Respond (Placeholder 전달 받은 뒤 변경)
String processor(const String &var) {
  if(var == "HELLO_FROM_TEMPLATE"){   // HELLO_FROM_TEMPLATE

    return F("ESP32 WEB SOCKET!");

  }else if(var == "LED_STATE"){       // LED_STATE (최초 LED 상태)
    
    if(digitalRead(LED)){
      return F("ON");
    }else{
      return F("OFF");
    }
  }
  return String();
}

void InitWebServer()
{
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
        { request->send(LittleFS, "/index.html", "text/html", false, processor); });

  // attach AsyncWebSocket
  ws.onEvent(onWsEvent);
  server.addHandler(&ws);

  server.serveStatic("/", LittleFS, "/");
  server.begin();
}

void onWsEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len)
{
  switch (type) {
    case WS_EVT_CONNECT:
      Serial.printf("WebSocket client [#%u] connected from %s\n", client->id(), client->remoteIP().toString().c_str());
      break;

    case WS_EVT_DISCONNECT:
      Serial.printf("WebSocket client [#%u] disconnected\n", client->id());
      break;

    case WS_EVT_DATA:
      Serial.printf("WebSocket client [#%u] message [%s]\n", client->id(), (char *)data);

      // 수신된 LED 제어(버튼) 데이터 확인 및 LED 상태 변환
      if (strncmp((char *)data, "ON", 2) == 0)
      {
        digitalWrite(LED, HIGH);
        Serial.println("LED ON");
      }
      else if (strncmp((char *)data, "OFF", 3) == 0)
      {
        digitalWrite(LED, LOW);
        Serial.println("LED OFF");
      }
      // 변경된 LED 상태 전송
      //client->text(digitalRead(LED)?"ON":"OFF");
      ws.textAll(String(digitalRead(LED)?"ON":"OFF"));  // 연결된 모든 소켓에 전송
      break;

    case WS_EVT_PONG:
      break;
    case WS_EVT_ERROR:
      break;
  }

}

/******************
* ARDUINO default *
*******************/

void setup() {

  // Serial port Enable
  Serial.begin(115200);

  pinMode(LED, OUTPUT);
  digitalWrite(LED, LOW);

  // Enable ESP32 FS
  initLittleFS();

  // Enable & Connect Wi-Fi
  initWiFi();

  // Enable & start WebServe
  InitWebServer();
}

void loop() {

}
