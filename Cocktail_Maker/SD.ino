#include <aWOT.h>
#include <ArduinoJson.h>
#include <SPI.h>
#include <SD.h>


void SDSetup () {
  Serial.begin(9600);
  while (!Serial) { 
  }
  Serial.print("Initializing SD card...");
  if (!SD.begin(10)) {
  Serial.println("initialization failed!");
  }
  Serial.println("initialization done.");
}


StaticJsonDocument<200> getDrinkData() {
  StaticJsonDocument<200> doc;
  File file = SD.open("drinks.txt");
  String json = "";
  if (file) {
    while (file.available()) {
      json += file.read();
    }
    file.close();
  }
  deserializeJson(doc, json);
  return doc;
}

StaticJsonDocument<200> getPumpData() {
  StaticJsonDocument<200> doc;
  File file = SD.open("pumps.txt");
  String json = "";
  if (file) {
    while (file.available()) {
      json += file.read();
    }
    file.close();
  }
  deserializeJson(doc, json);
  return doc;
}

void updateDrinkData(StaticJsonDocument<500> doc) {
//  JsonArray drinks = doc.as<JsonArray>();
//  for (JsonObject drink : drinks) {
//    const char* drinkStr = drink["name"];
//    Serial.println(drinkStr);
//  }
    File file = SD.open("drinks.txt", FILE_WRITE);
    serializeJson(doc, file);
    file.close();
    

}
