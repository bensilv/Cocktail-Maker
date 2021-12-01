#include <aWOT.h>
#include <ArduinoJson.h>

StaticJsonDocument<200> getDrinkData() {
  StaticJsonDocument<200> doc;
  deserializeJson(doc, "{\"name\":\"John Smith\",\"password\":\"secret\"}");
  return doc;
}
