/*
* ESP32 LittleFS + 파일 업로드 간단 테스트 
* '/' 경로에 업로드 된 파일 목록을 읽고 출력 & .txt파일인 경우 내용을 출력합니다.
* 
* > LittleFS 업로드 프로그램 (arduino-littlefs-upload)
* https://github.com/earlephilhower/arduino-littlefs-upload/releases
*
* > 사용법 참조 - 엘레파츠 블로그
* https://blog.naver.com/elepartsblog/223494337611
*/
#include "LittleFS.h"

String filename;
int Number = 1;

void setup() {

  Serial.begin(115200);

  // LittleFS enable
  if (!LittleFS.begin(true)) {
    Serial.println("An Error has occurred while mounting LittleFS");
  }

  Serial.println("\n\n--- ESP32 LittleFS TEST ---\n");

  File root = LittleFS.open("/");
  File file = root.openNextFile();

  while(file){
    
    // 파일&폴더 이름 출력
    filename = file.name();

    Serial.print((String)(Number++) + " : ");
    Serial.println(filename);

    // .txt 파일인 경우 내용 출력
    if(filename.endsWith(".txt")){
          
      Serial.print("└ text data : ");

      while(file.available()){
        Serial.write(file.read());
      } 
      Serial.println("");
    }

    file = root.openNextFile();
  }
}

void loop() {

}