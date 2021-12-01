#include <ArduinoJson.h>

StaticJsonDocument<200> getDrinkData();
StaticJsonDocument<200> getPumpData();

void updateDrinkData(StaticJsonDocument<500> drinks);
