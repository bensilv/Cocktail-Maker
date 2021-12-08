volatile int* curr_pumps;
volatile float* curr_ounces;
volatile int num_pumps_running;
volatile int num_ingredients; 
volatile boolean mixing;


Servo myservo;
int last_emergency_stop = 0;


void mechanicsSetup() {
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(BUTTON, INPUT);
  pinMode(PUMP_ONE, OUTPUT);
  pinMode(PUMP_TWO, OUTPUT);
  pinMode(PUMP_THREE, OUTPUT);
  pinMode(PUMP_FOUR, OUTPUT);
  pinMode(DC_MOTOR, OUTPUT);
  pinMode(GREEN_LED, OUTPUT);
  pinMode(RED_LED, OUTPUT);
  myservo.attach(SERVO);
  #ifndef TESTING
    myservo.write(40);
    attachInterrupt(digitalPinToInterrupt(BUTTON), emergency_stop, RISING);
  #endif
}



volatile void start_pump() {
  int next_index = num_ingredients - num_pumps_running;
  int pump = curr_pumps[next_index];
  float amount = curr_ounces[next_index];
  #ifndef TESTING
    digitalWrite(pump, HIGH);
  #endif
  start_timer(ounces_to_millis(amount));

}


volatile void stop_pump(){
  int curr_index = num_ingredients - num_pumps_running;
  int pump = curr_pumps[curr_index];
  #ifndef TESTING
    digitalWrite(pump, LOW);
  #endif
  num_pumps_running --;
  if (num_pumps_running != 0){
    start_pump();
  }
}


void start_pumps(recipe ordered_recipe) {
    num_pumps_running = ordered_recipe.num_ingredients;
    num_ingredients = ordered_recipe.num_ingredients;

    curr_ounces = new volatile float[6];
    curr_pumps = new volatile int[6];
    for (int i=0; i<num_ingredients;i++){
      curr_ounces[i] = ordered_recipe.ingredients[i].amount;
      curr_pumps[i] = ordered_recipe.ingredients[i].pump;
    }
    
    callback = &stop_pump;
    vars.recipe_loaded = false;
    start_pump();

}

void readVolatileVals(){
  vars.num_pumps_running = num_pumps_running;
  vars.mixing = mixing;
}


void change_mixer_position(mixer_position new_pos) {
  if (vars.stopped) return;
  #ifndef TESTING
    myservo.write(new_pos);
  #endif
  delay(1000);
  vars.mixer_pos = new_pos;
}


volatile void stop_mixer(){
  #ifndef TESTING
    analogWrite(DC_MOTOR, 0);
  #endif
  mixing = false;
}

void start_mixer() {
  callback = &stop_mixer;
  mixing = true;
  #ifndef TESTING
    analogWrite(DC_MOTOR, 200);
  #endif
  start_timer(1000);
}


float ounces_to_millis(float amount) { //note: amount is in ounces  
  return amount*600;
}

void emergency_stop_ISR() {

  // Checking for noise. A true button press would pass this loop.
  for(int i=0; i<100; i++){
    if (digitalRead(BUTTON) != 1){
      return;
    }
  }

  // Check for noise again. Sometimes the ISR is triggered multiple times in a row. 
  int curr_time = millis();
  if (curr_time - last_emergency_stop < 500 ){
    return;
  }
  emergency_stop();
}


void emergency_stop(){
  last_emergency_stop = millis();
  if (vars.stopped == true) {
    reset_state();
  } else {
    vars.stopped = true;
    
    //turn off all pumps, turn off motor + servo
    #ifndef TESTING
      analogWrite(DC_MOTOR, 0);
      digitalWrite(PUMP_ONE, LOW);
      digitalWrite(PUMP_TWO, LOW);
      digitalWrite(PUMP_THREE, LOW);
      digitalWrite(PUMP_FOUR, LOW);
      myservo.write(40);
    #endif
    vars.mixer_pos = MIXER_UP;
  }
}

void displayGreenLED(){
  #ifndef TESTING
    digitalWrite(GREEN_LED, HIGH);
    digitalWrite(RED_LED, LOW);
  #endif
}

void displayRedLED(){
  #ifndef TESTING
    digitalWrite(RED_LED, HIGH);
    digitalWrite(GREEN_LED, LOW);
  #endif
}

void reset_state() {
  vars.stopped = false;
  vars.recipe_loaded = false;
  vars.num_pumps_running = 0;
  vars.mixing = false;
  vars.mixer_pos = MIXER_UP;
  #ifndef TESTING
    myservo.write(40);
  #endif
  mixing = false;
  num_pumps_running = 0;
}
