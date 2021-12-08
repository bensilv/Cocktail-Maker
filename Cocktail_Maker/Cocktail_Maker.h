#include <ArduinoJson.h>
#include <WiFi101.h>
#include <aWOT.h>
#include <SPI.h>
#include <SD.h>
#include <Servo.h>

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
  sPUMPING = 3,
  sMIXER_LOWERING = 4,
  sMIXING = 5,
  sMIXER_RAISING = 6,
  sALL_STOP = 7,
} state;

typedef enum {
  MIXER_UP = 40,
  MIXER_DOWN = 120,
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
  int num_pumps_running;
  boolean stopped;
  boolean server_running;
}state_variables;

state_variables vars;
state CURR_STATE;
volatile state_variables *vars_p = &vars;
void (*callback)(void);
