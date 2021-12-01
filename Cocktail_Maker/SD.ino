#include <aWOT.h>
#include <ArduinoJson.h>

StaticJsonDocument<200> getDrinkData() {
  StaticJsonDocument<200> doc;
  deserializeJson(doc, "{\"name\":\"John Smith\",\"password\":\"secret\"}");
  return doc;
}

StaticJsonDocument<200> getPumpData() {
  StaticJsonDocument<200> doc;
  deserializeJson(doc, "{\"name\":\"John Smith\",\"password\":\"secret\"}");
  return doc;
}

void updateDrinkData(StaticJsonDocument<500> doc) {\
  JsonArray drinks = doc.as<JsonArray>();
  for (JsonObject drink : drinks) {
    const char* drinkStr = drink["name"];
    Serial.println(drinkStr);
  }
}
