ingredient ingredients[2];
recipe myRecipe;


void runTestSuite() {
  int passed = 0; 
  int total = 21; 
  
  ingredients[0] = ingredient{PUMP_ONE, 0.5};
  ingredients[1] = ingredient{PUMP_TWO, 0.5};
  myRecipe = {ingredients, 2}; 

  Serial.println("\n\n----- FSM Tests -----\n");

  passed += one_to_one_test(); 
  passed += one_to_two_test(); 
  passed += two_to_two_test(); 
  passed += two_to_three_test(); 
  delay(1000);
  passed += three_to_three_test(); 
  passed += three_to_four_test(); 
  passed += four_to_four_test(); 
  passed += four_to_five_test(); 
  passed += five_to_five_test(); 
  passed += two_to_five_test();
  passed += three_to_five_test();
  passed += four_to_two_test(); 
  passed += five_to_two_test();

  Serial.println("\n\n----- Mechanics Tests -----\n");
  passed += ounces_to_millis_test(); 

  Serial.println("\n\n----- Server Tests -----\n");
  

  Serial.print(passed);
  Serial.print(" of ");
  Serial.print(total);
  Serial.println(" tests passed!");
}

//TRANSITION: 1-1
int one_to_one_test() {
 
    state_variables vars_new = {
    MIXER_UP, //mixer pos
    false, // mixing
    false, //recipe_loaded
    myRecipe, //pointer to recipe
    0, //num pumps
    false, //stopped
    false,
  };

  state next_state = update_fsm(sSETUP, vars_new);
  return testAssert(next_state == sSETUP, "1-1", "incorrect state transition");
}

//TRANSITION: 1-2
int one_to_two_test() {

    
    state_variables vars_new = {
    MIXER_UP, //mixer pos
    false, // mixing
    false, //recipe_loaded
    myRecipe, //pointer to recipe
    0, //num pumps
    false, //stopped
    true,
  };

  state next_state = update_fsm(sSETUP, vars_new);

  return testAssert(next_state == sREADY_TO_MAKE, "1-2", "incorrect state transition");
}

//TRANSITION: 2-2
int two_to_two_test() {

    state_variables vars_new = {
    MIXER_UP, //mixer pos
    false, // mixing
    false, //recipe_loaded
    myRecipe, //pointer to recipe
    0, //num pumps
    false, //stopped
    true,
  };

  state next_state = update_fsm(sREADY_TO_MAKE, vars_new);

  return testAssert(next_state == sREADY_TO_MAKE, "2-2", "incorrect state transition");
}

//TRANSITION: 2-3 
int two_to_three_test() { 
   
    state_variables vars_new = {
    MIXER_UP, //mixer pos
    false, // mixing
    true, //recipe_loaded
    myRecipe, //pointer to recipe
    0, //num pumps
    false, //stopped
    true,
  };

  state next_state = update_fsm(sREADY_TO_MAKE, vars_new);
  readVolatileVals();
  return testAssert(next_state == sPUMPING, "2-3 A", "incorrect state transition") +
    testAssert(vars.num_pumps_running == 2, "2-3 B", "number of pumps running incorrect");
}

//TRANSITION: 3-3
int three_to_three_test() {
    
    state_variables vars_new = {
    MIXER_UP, //mixer pos
    false, // mixing
    true, //recipe_loaded
    myRecipe, //pointer to recipe
    1, //num pumps
    false, //stopped
    true,
  };
  //We can try to do some sort of delay system that matches the ISR to test num_pumps_running is decremented
  state next_state = update_fsm(sPUMPING, vars_new);
  return testAssert(next_state == sPUMPING, "3-3", "incorrect state transition");
}

//TRANSITION: 3-4 
int three_to_four_test() {

    
    state_variables vars_new = {
    MIXER_UP, //mixer pos
    false, // mixing
    true, //recipe_loaded
    myRecipe, //pointer to recipe
    0, //num pumps
    false, //stopped
    true,
  };

  state next_state = update_fsm(sPUMPING, vars_new);
  delay(1000);
  readVolatileVals();
  return testAssert(next_state == sMIXING, "3-4 A", "incorrect state transition") + 
    testAssert(vars.mixer_pos == MIXER_DOWN, "3-4 B", "mixer was not lowered") + 
    testAssert(vars.num_pumps_running == 0, "3-4 C", "not all pumps completed") +
    testAssert(vars.mixing == true, "3-4 D", "mixing did not begin");
}

//TRANSITION: 4-4 
int four_to_four_test() {
    
    state_variables vars_new = {
    MIXER_DOWN, //mixer pos
    true, // mixing
    true, //recipe_loaded
    myRecipe, //pointer to recipe
    0, //num pumps
    false, //stopped
    true,
  };

  state next_state = update_fsm(sMIXING, vars_new);
  return testAssert(next_state == sMIXING, "4-4", "incorrect state transition");
}

//TRANSITION: 4-2 
int four_to_two_test() {

    
    state_variables vars_new = {
    MIXER_DOWN, //mixer pos
    false, // mixing
    true, //recipe_loaded
    myRecipe, //pointer to recipe
    0, //num pumps
    false, //stopped
    true,
  };

  state next_state = update_fsm(sMIXING, vars_new);
  readVolatileVals();
  return testAssert(next_state == sREADY_TO_MAKE, "4-2 A", "incorrect state transition") + 
    testAssert(vars.mixer_pos == MIXER_UP, "4-2 B", "mixer was not lowered") + 
    testAssert(vars.mixing == false, "4-2 C", "mixer was not stopped");
}

//TRANSITION: 5-2 
int five_to_two_test() {

    
    state_variables vars_new = {
    MIXER_UP, //mixer pos
    false, // mixing
    true, //recipe_loaded
    myRecipe, //pointer to recipe
    0, //num pumps
    false, //stopped
    true,
  };

  state next_state = update_fsm(sALL_STOP,  vars_new);
  return testAssert(next_state == sREADY_TO_MAKE, "5-2", "incorrect state transition");
}

//TRANSITION: 2-5
int two_to_five_test() {
  
    
    state_variables vars_new = {
    MIXER_UP, //mixer pos
    false, // mixing
    true, //recipe_loaded
    myRecipe, //pointer to recipe
    0, //num pumps
    true, //stopped
    true,
  };

  state next_state = update_fsm(sREADY_TO_MAKE, vars_new);
  return testAssert(next_state == sALL_STOP, "2-5", "incorrect state transition");
}

//TRANSITION: 3-5
int three_to_five_test() {
    
    state_variables vars_new = {
    MIXER_UP, //mixer pos
    false, // mixing
    true, //recipe_loaded
    myRecipe, //pointer to recipe
    0, //num pumps
    true, //stopped
    true,
  };

  state next_state = update_fsm(sPUMPING, vars_new);
  return testAssert(next_state == sALL_STOP, "3-5", "incorrect state transition");
}

//TRANSITION: 4-5
int four_to_five_test() {
    
    state_variables vars_new = {
    MIXER_UP, //mixer pos
    false, // mixing
    true, //recipe_loaded
    myRecipe, //pointer to recipe
    0, //num pumps
    true, //stopped
    true,
  };

  state next_state = update_fsm(sMIXING, vars_new);
  return testAssert(next_state == sALL_STOP, "4-5", "incorrect state transition");
}

//TRANSITION: 5-5
int five_to_five_test() {
    
    state_variables vars_new = {
    MIXER_UP, //mixer pos
    false, // mixing
    true, //recipe_loaded
    myRecipe, //pointer to recipe
    0, //num pumps
    true, //stopped
    true,
 
  };

  state next_state = update_fsm(sALL_STOP, vars_new);
  return testAssert(next_state == sALL_STOP, "5-5", "incorrect state transition");
}

int ounces_to_millis_test() {
  return testAssert(ounces_to_millis(5) == 3000, "ounces_to_millis_a", "expected 3000") +
    testAssert(ounces_to_millis(0) == 0, "ounces_to_millis_b", "expected 0");  
}


int testAssert(boolean logic, String testName, String error) {
  if (logic) {
    Serial.print("TEST ");
    Serial.print(testName);
    Serial.println(" : PASSED"); 
    return 1; 
  } else {
    Serial.print("TEST ");
    Serial.print(testName);
    Serial.print(" : FAILED, ");
    Serial.println(error); 
    return 0; 
  }
}
