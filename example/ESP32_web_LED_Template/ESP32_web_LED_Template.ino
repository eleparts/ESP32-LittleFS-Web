/*
* ESP32로 웹 서버를 실행, 버튼으로 각각의 웹 페이지에 접속하면 LED를 on/off해 줍니다.
* LittleFS을 사용해 ESP32 내부 공간에 별도의 HTML 및 JS 파일을 저장하고 이 파일로 웹 페이지를 실행해 줍니다.
*
* ※ [버튼 -> 페이지 이동]으로 동작하므로 새로고침, 주소 변경으로 접속하는 경우에도 반응합니다.
*/
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <LittleFS.h>

#define LED_PIN 2 // 보드 내 LED 

/* 사용할 WIFI 접속 정보 입력 */
const char *ssid     = "WIFI_SSID"; 
const char *password = "WIFI_PASSWORD";

AsyncWebServer server(80);

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

// TEMPLATE Respond (Placeholder 전달 받은 뒤 해당하는 부분을 변경)
String processor(const String &var) {
  if(var == "HELLO_FROM_TEMPLATE"){   // HELLO_FROM_TEMPLATE
  
    return F("ESP32 WEB LED CONTROL");
  
  }else if(var == "LED_STATE"){       // LED_STATE
    
    if(digitalRead(LED_PIN)){
      return F("ON");
    }else{
      return F("OFF");
    }
  }

  return String();
}

// Initialize WebServer
void InitWebServer() {

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){ 
    request->send(LittleFS, "/index.html", "text/html", false, processor); 
  });
  
  // -/LEDON 페이지 접속 시 LED ON
  server.on("/LEDON", HTTP_GET, [](AsyncWebServerRequest *request){ 
    digitalWrite(LED_PIN, HIGH);
    Serial.println("LED ON");
    request->send(LittleFS, "/index.html", "text/html", false, processor); 
  });

  // -/LEDOFF 페이지 접속 시 LED OFF
  server.on("/LEDOFF", HTTP_GET, [](AsyncWebServerRequest *request){ 
    digitalWrite(LED_PIN, LOW);
    Serial.println("LED OFF");
    request->send(LittleFS, "/index.html", "text/html", false, processor); 
  });

  server.serveStatic("/", LittleFS, "/");
  server.begin();
  
}

/******************
* ARDUINO default *
*******************/

void setup() {

  // Serial port Enable
  Serial.begin(115200);

  pinMode(LED_PIN, OUTPUT);

  // Enable ESP32 FS
  initLittleFS();

  // Enable & Connect Wi-Fi
  initWiFi();

  // Enable & start WebServe
  InitWebServer();
}

void loop() {
}
