#include <aWOT.h>
#include <ArduinoJson.h>

StaticJsonDocument<10000> getDrinkData() {
  StaticJsonDocument<10000> doc;
  deserializeJson(doc, "[{\"name\":\"John Smith\",\"description\":\"desc\",\"recipe\":{\"ingredient1\":\"amount1\"}},"
                       "{\"name\":\"Drink 2\",\"description\":\"description 2\",\"recipe\":{\"ing2\":\"amount2\"}}]");
  return doc;
}

StaticJsonDocument<10000> getPumpData() {
  StaticJsonDocument<10000> doc;
  deserializeJson(doc, "[\"pump1drink\",\"pump2drink\"]");
  return doc;
}

void updateDrinkData(StaticJsonDocument<500> doc) {\
  JsonArray drinks = doc.as<JsonArray>();
  for (JsonObject drink : drinks) {
    const char* drinkStr = drink["name"];
    Serial.println(drinkStr);
  }
}

void updatePumpData(StaticJsonDocument<500> doc) {\
  JsonArray pumps = doc.as<JsonArray>();
  for (JsonVariant pump : pumps) {
    Serial.println(pump.as<const char*>());
  }
}
