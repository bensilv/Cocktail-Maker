#include <ArduinoJson.h>
#include <WiFi101.h>
#include <aWOT.h>
#include <SPI.h>
#include <SD.h>

typedef enum {
  PUMP_ONE = 6,
  PUMP_TWO = 7,
  PUMP_THREE = 11,
  PUMP_FOUR = 12,
  DC_MOTOR = 3,
  SERVO = 2,
  BUTTON = A1,
  GREEN_LED = 5,
  RED_LED = 1,
  } pin;

typedef enum {
  sSETUP = 1,
  sREADY_TO_MAKE = 2,
  sSTART_PUMP = 3,
  sPUMPING = 4,
  sMIXER_LOWERING = 5,
  sMIXING = 6,
  sMIXER_RAISING = 7,
  sALL_STOP = 8,
} state;

typedef enum {
  MIXER_UP = 1,
  MIXER_DOWN = 2,
  
}mixer_position;


typedef struct {
  int pump;
  float amount;
} ingredient;


typedef struct {
  ingredient *ingredients;
  int num_ingredients;
} recipe;


typedef struct {
  mixer_position mixer_pos;
  boolean mixing;
  boolean recipe_loaded;
  recipe curr_recipe;
  int num_pumps_started;
  int num_pumps_finished;
  boolean stopped;
}state_variables;


StaticJsonDocument<1000> getDrinkData();
StaticJsonDocument<1000> getPumpData();

void updateDrinkData(StaticJsonDocument<1000> drinks);
void SDSetup ();
state update_fsm(state cur_state, boolean server_running);
char* s2str(state s);
void start_pumps(recipe ordered_recipe);
int get_pump_num (String ingredient);
volatile int num_pumps_finished;
