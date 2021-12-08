#include "Cocktail_Maker.h"

// UNCOMMENT FOR TESTING
#define TESTING

// UNCOMMENT IF SD CARD PRESENT
//#define SDPRESENT

void setup(){
  CURR_STATE = sSETUP;
  Serial.begin(9600);
  while (!Serial);
  SDSetup();
  serverSetup();
  mechanicsSetup();
  setupTimers();
  Serial.println(s2str(CURR_STATE));

  #ifdef TESTING
    runTestSuite();
  #endif
}

void loop() {
  petWatchdog();
  readVolatileVals();
  CURR_STATE = update_fsm(CURR_STATE, vars);
  handleClient();
  delay(10);
}
