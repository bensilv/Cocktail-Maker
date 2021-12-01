


void SDSetup () {
  Serial.print("Initializing SD card...");
//  pinMode(4, OUTPUT); // change this to 53 on a mega  // don't follow this!!
//  digitalWrite(4, HIGH); // Add this line
  if (!SD.begin(4)) {
  Serial.println("initialization failed!");
  }
  Serial.println("initialization done.");
}


StaticJsonDocument<1000> getDrinkData() {
  StaticJsonDocument<1000> doc;
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

StaticJsonDocument<1000> getPumpData() {
  StaticJsonDocument<1000> doc;
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

void updateDrinkData(StaticJsonDocument<1000> doc) {
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

void updatePumpData(StaticJsonDocument<1000> doc) {
  JsonArray pumps = doc.as<JsonArray>();
  Serial.println("updating pumps sd");
  SD.remove("pumps.txt");
  File file = SD.open("pumps.txt", FILE_WRITE);
  serializeJson(doc, file);
  serializeJson(doc, Serial);
  file.close();
}
