#include <ArduinoJson.h>
#include <WiFi101.h>
#include <aWOT.h>
#include <SPI.h>
#include <SD.h>


typedef enum {
  sDISP_COUNTDOWN = 1,
  sWAIT_AFTER_ROT = 2,
  sMOV = 3,
  sWAIT_AFTER_MOV = 4,
  sROT = 5,
  sWAIT_FOR_BUT = 6,
  sGAME_OVER = 7,
} state;

typedef struct {
  int pump;
  float amount;
} ingredient;


typedef struct {
  ingredient ingredients[];
} recipe;


StaticJsonDocument<1000> getDrinkData();
StaticJsonDocument<1000> getPumpData();

void updateDrinkData(StaticJsonDocument<1000> drinks);
void SDSetup ();
