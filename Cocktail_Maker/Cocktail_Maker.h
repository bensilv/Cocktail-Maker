#include <ArduinoJson.h>

StaticJsonDocument<10000> getDrinkData();
StaticJsonDocument<10000> getPumpData();

void updateDrinkData(StaticJsonDocument<500> drinks);
void updatePumpData(StaticJsonDocument<500> drinks);
