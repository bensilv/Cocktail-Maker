#include "Cocktail_Maker.h"
#define TESTING = false;

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
  //runTestSuite();
  #endif
  
}

void loop() {
  petWatchdog();
  //pass by value, vars gets copied so the vars passed in are a "snapshot"
  //same with CURR_STATE
  readVolatileVals();
  CURR_STATE = update_fsm(CURR_STATE, vars);
  handleClient();
  delay(10);
}
