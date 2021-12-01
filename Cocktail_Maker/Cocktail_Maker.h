#include <ArduinoJson.h>

StaticJsonDocument<1000> getDrinkData();
StaticJsonDocument<1000> getPumpData();

void updateDrinkData(StaticJsonDocument<1000> drinks);
void SDSetup ();
