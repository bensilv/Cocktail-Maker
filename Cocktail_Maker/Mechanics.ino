volatile int[] curr_pumps;
volatile float[] curr_ounces;
volatile int num_pumps_running;
volatile int num_ingredients; 


Servo myservo;
int last_emergency_stop = 0;



void mechanicsSetup() {
  //mechanical initializations
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
  myservo.write(40);
  //attachInterrupt(digitalPinToInterrupt(BUTTON), emergency_stop, RISING);
}



void start_pump() {
  int next_index = num_ingredients - num_pumps_running;
  int pump = curr_pumps[next_index];
  float amount = curr_ounces[next__index];
 
  digitalWrite(pump, HIGH);

  Serial.print("millis: ");
  Serial.println(millis());
  Serial.print("starting pump: ");
  Serial.println(pump);
  start_timer(ounces_to_millis(amount));
//  delay(5000);
//  stop_pump();
}


void stop_pump(){
  int curr_index = num_ingredients - num_pumps_running;
  int pump = curr_pumps[curr_index];
 
 

  digitalWrite(pump, LOW);
  num_pumps_running --;
  Serial.print("millis: ");
  Serial.println(millis());
  Serial.print("stopping pump: ");
  Serial.println(pump);
  
 
  if (num_pumps_running != 0){
    start_pump();
  }
}


void start_pumps(recipe ordered_recipe) {
    num_pumps_running = ordered_recipe.num_ingredients;
    num_ingredients = ordered_recipe.num_ingredients;
    for (int i=0; i<num_ingredients;i++){
      curr_ounces[i] = ordered_recipe.ingredients[i].amount;
      curr_pumps[i] = ordered_recipe.ingredients[i].pump;
    }
    
    callback = &stop_pump;
    vars.recipe_loaded = false;
    start_pump();

}


void change_mixer_position(mixer_position new_pos) {
  if (vars.stopped) return;
  myservo.write(new_pos);
  delay(1000);
  vars.mixer_pos = new_pos;
}


void stop_mixer(){
  analogWrite(DC_MOTOR, 0);
  vars_p->mixing = false;
}

void start_mixer() {
  callback = &stop_mixer;
  vars.mixing = true;
  analogWrite(DC_MOTOR, 200);
  //start_timer(1000);
  delay(1000);
  stop_mixer();
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
    analogWrite(DC_MOTOR, 0);
    digitalWrite(PUMP_ONE, LOW);
    digitalWrite(PUMP_TWO, LOW);
    digitalWrite(PUMP_THREE, LOW);
    digitalWrite(PUMP_FOUR, LOW);
    myservo.write(40);
    vars.mixer_pos = MIXER_UP;
  }
}

void displayGreenLED(){
  digitalWrite(GREEN_LED, HIGH);
  digitalWrite(RED_LED, LOW);
}

void displayRedLED(){
  digitalWrite(RED_LED, HIGH);
  digitalWrite(GREEN_LED, LOW);
}

void reset_state() {
  vars.stopped = false;
  vars.recipe_loaded = false;
  vars.num_pumps_running = 0;
  vars.mixing = false;
  vars.mixer_pos = MIXER_UP;
  myservo.write(40);
}


void delay_helper(int mils){
  int start_time = millis();
  while(millis() - start_time < mils){
    if (digitalRead(BUTTON) == 1){
      emergency_stop_ISR();
    }   
  }
}
