#include <aWOT.h>
#include <ArduinoJson.h>
#include <SPI.h>
#include <SD.h>


void SDSetup () {
  Serial.print("Initializing SD card...");
//  pinMode(4, OUTPUT); // change this to 53 on a mega  // don't follow this!!
//  digitalWrite(4, HIGH); // Add this line
  if (!SD.begin(4)) {
  Serial.println("initialization failed!");
  }
  Serial.println("initialization done.");
}


StaticJsonDocument<200> getDrinkData() {
  StaticJsonDocument<200> doc;
  if (SD.exists("drinks.txt")) {
    File file = SD.open("drinks.txt");
    String json = "";
    if (file) {
      while (file.available()) {
        json += (char) file.read();
      }
      file.close();
   }
   deserializeJson(doc, json);
  } else {
    String json = "[]";
    deserializeJson(doc, json);
  }
  return doc;
}

StaticJsonDocument<200> getPumpData() {
  StaticJsonDocument<200> doc;
  if (SD.exists("pumps.txt")) {
    File file = SD.open("pumps.txt");
    String json = "";
    if (file) {
      while (file.available()) {
        json += (char) file.read();
      }
      file.close();
    }
    deserializeJson(doc, json);
  } else {
    String json = "[]";
    deserializeJson(doc, json);
  }
  return doc;
}

void updateDrinkData(StaticJsonDocument<500> doc) {
  JsonArray drinks = doc.as<JsonArray>();
  for (JsonObject drink : drinks) {
    const char* drinkStr = drink["name"];
    const char* descriptionStr = drink["description"];
    Serial.println(drinkStr);
    Serial.println(descriptionStr);
  }
  Serial.println("updating drinks sd");
  SD.remove("drinks.txt");
  File file = SD.open("drinks.txt", FILE_WRITE);
  serializeJson(doc, file);
  serializeJson(doc, Serial);
  file.close();
    

}
