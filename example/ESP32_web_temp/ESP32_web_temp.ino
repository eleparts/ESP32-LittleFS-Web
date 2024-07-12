/*
* ESP32로 웹 서버를 실행, AM2301B(AHT20 + Case) 온습도 센서로 온습도를 측정해 웹페이지에 출력해 줍니다.
* LittleFS을 사용해 ESP32 내부 공간에 별도의 HTML 및 JS 파일을 저장하고 이 파일로 웹 페이지를 실행해 줍니다.
* 
* AM2301B / https://www.eleparts.co.kr/EPY4VWFC
* VCC (RED)    : 3.3 / 5V
* SDA (Yellow) : GPIO 21
* GND (Black)  : GND
* SCL (White)  : GPIO 22
* ※ 케이블 타입 제품으로 별도의 작업 후 연결하거나 ESP32 모듈을 브레드 보드 등에 연결후 납이 발려져 있는 케이블 끝부분을 꽂아 사용
*/
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <LittleFS.h>
#include <DFRobot_AHT20.h>      //  라이브러리 매니저에서 'DFRobot_AHT20' 설치 

/* 사용할 WIFI 접속 정보 입력 */
const char *ssid     = "WIFI_SSID"; 
const char *password = "WIFI_PASSWORD";

DFRobot_AHT20 am2301b;
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

// Initialize AM2301B
void initAM2301B() {
  int status = 0;

  while((status = am2301b.begin()) != 0){
    Serial.print("AM2301B sensor initialization failed. error status : ");
    Serial.println(status);
    delay(1000);
  }
  Serial.println("AM2301B sensor Enabled");
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

// TEMPLATE Respond
String processor(const String &var) {
  if(var == "HELLO_FROM_TEMPLATE")
    return F("ESP32 + AM2301B Temp&Humid");
  return String();
}

// Initialize WebServer
void initWebServer() {

  server.addHandler(&events);

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){ 
    request->send(LittleFS, "/index.html", "text/html", false, processor); 
  });

  server.serveStatic("/", LittleFS, "/");
  server.begin();
}

// USER Event
void myEvent() {
  static unsigned long previousTime, currentTime;
  static const unsigned long  interval = 1000;

  int temperature = 0, humidity = 0;

  currentTime = millis();

  // (interval)ms 마다 실행
  if (currentTime - previousTime >= interval) {
    previousTime = currentTime;

    getAM2301B(&temperature, &humidity);

    // EventSource 데이터 전송 (단방향&텍스트 전용)
    events.send(String(temperature).c_str(), "myeventTemp", millis());
    events.send(String(humidity).c_str(), "myeventHumid", millis());
  }
}

// AM2301B 데이터 수신, 출력
int getAM2301B(int* temperature, int* humidity) {

  if (am2301b.startMeasurementReady(true)) {

    *temperature = am2301b.getTemperature_C();
    *humidity    = am2301b.getHumidity_RH();

    Serial.print("Temperature: ");
    Serial.print(*temperature);
    Serial.print(" °C\tHumidity: ");
    Serial.print(*humidity);
    Serial.println(" %");

    return 0;
  }
  
  Serial.println("AM2301B sensor Measurement failed");
  return 1;
}


/******************
* ARDUINO default *
*******************/

void setup() {


  // Serial port Enable
  Serial.begin(115200);

  // Enable ESP32 FS
  initLittleFS();

  // Enable AM2301B (I2C)
  initAM2301B();

  // Enable & Connect Wi-Fi
  initWiFi();

  // Enable & start WebServe
  initWebServer();
}


void loop() {

  myEvent();

}
