
void SDSetup () {
  Serial.print("Initializing SD card...");
  if (!SD.begin(4))      {
  Serial.println("initialization failed!");
  }
  Serial.println("initialization done.");
}


File open_truncate(String filename){
  SD.remove(filename);
  return SD.open(filename, FILE_WRITE);
}

StaticJsonDocument<1000> jsonDocFromFile(String filename) {
  StaticJsonDocument<1000> doc;
  String json;
  if (SD.exists(filename)) {
    File file = SD.open(filename);
    json = jsonListFromFile(file);
    file.close();
  } else {
    json = "[]";
  }
  deserializeJson(doc, json);
  return doc;
}

String jsonListFromFile(File file){
  String json = "";
    if (file) {
      while (file.available()) {
        json += (char) file.read();
      }
    }
  return json;
}


StaticJsonDocument<1000> getPumpData() {
  return jsonDocFromFile("pumps.txt");
}

StaticJsonDocument<1000> getDrinkData() {
  return jsonDocFromFile("drinks.txt");
}


void updateDrinkData(StaticJsonDocument<1000> doc) {
  JsonArray drinks = doc.as<JsonArray>();
  for (JsonObject drink : drinks) {
    const char* drinkStr = drink["name"];
    const char* descriptionStr = drink["description"];
  }
  File file = open_truncate("drinks.txt");
  serializeJson(doc, file);
  file.close();
}

void updatePumpData(StaticJsonDocument<1000> doc) {
  JsonArray pumps = doc.as<JsonArray>();
  File file = open_truncate("pumps.txt");
  serializeJson(doc, file);
  file.close();
}
