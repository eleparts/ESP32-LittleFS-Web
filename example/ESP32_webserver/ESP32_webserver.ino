/*
* ESP32 LittleFS + 파일 업로드 웹 서버 실행 예제
* LittleFS 업로드 프로그램을 사용해 ESP32 내부 공간에 별도의 HTML 및 JS 파일을 저장하고 이 파일로 웹 페이지를 실행해 줍니다.
* EventSource(Server-Sent Events)로 서버 -> 브라우저로 텍스트를 전달, 부팅 직후부터 카운트를 세는 간단한 동작을 합니다.
* ※ 비동기식 단방향 텍스트 이벤트 전달 방식으로 동기식/양방향 전달이 필요하다면 WebSockets을 사용해야 합니다.
* 
* 설치 필요 라이브러리
* ESPAsyncWebServer / ESP용 비동기식 웹서버 라이브러리
* https://github.com/me-no-dev/ESPAsyncWebServer
*
* AsyncTCP / ESP32용 추가 확장
* https://github.com/me-no-dev/AsyncTCP
*/
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <LittleFS.h>

/* 사용할 WIFI 접속 정보 입력 */
const char *ssid = "WIFI_SSID"; 
const char *password = "WIFI_PASSWORD";

AsyncWebServer server(80);
AsyncEventSource events("/events");


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

// TEMPLATE Respond (Placeholder 전달 받아 내용 변경)
String processor(const String &var) {
  if(var == "HELLO_FROM_TEMPLATE")
    return F("ESP32 WEB PAGE!");
  return String();
}

// Initialize WebServer
void InitWebServer() {

  server.addHandler(&events);

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(LittleFS, "/index.html", "text/html", false, processor); 
  });

  server.serveStatic("/", LittleFS, "/");
  server.begin();
  
}

// USER Event: 주기적으로 실행할 이벤트
void myEvent() {
  static unsigned long previousTime, currentTime;
  static const unsigned long  interval = 1000;
  static unsigned int value = 0;

  currentTime = millis();

  // (interval)ms 마다 실행
  if (currentTime - previousTime >= interval) {
    previousTime = currentTime;

    // EventSource 데이터 전송 (단방향&텍스트)
    events.send(String(value++).c_str(), "myevent", millis());
  }
}


/******************
* ARDUINO default *
*******************/

void setup() {

  // Serial port Enable
  Serial.begin(115200);

  // Enable ESP32 FS
  initLittleFS();

  // Enable & Connect Wi-Fi
  initWiFi();

  // Enable & start WebServe
  InitWebServer();
}

void loop() {

  myEvent();

}
