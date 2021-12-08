

void setupFSM(){
  vars = state_variables{
    MIXER_UP, //mixer pos
    false, // mising
    NULL, //pointer to recipe
    0, //num pumps
    false, //stopped
    false //server_running
  };
}

state update_fsm(state cur_state, state_variables vars) {
               
  state next_state;

  if (vars.stopped == true and cur_state != sSETUP){
    next_state = sALL_STOP;
    if (cur_state != next_state) {
      Serial.println(s2str(next_state));
    }
    displayRedLED();
    return next_state;
  }
  
  switch(cur_state) {
  case sSETUP:
    if (vars.server_running){ 
      next_state = sREADY_TO_MAKE;
      displayGreenLED();
    } else {
      next_state = sSETUP;
    }
    break;
  case sREADY_TO_MAKE:
    if (vars.recipe_loaded){
      start_pumps(vars.curr_recipe);
      next_state = sPUMPING;
    } else {
      next_state = sREADY_TO_MAKE;
    }
    break;
  case sPUMPING:
    if (vars.num_pumps_running == 0){
      next_state = sMIXING;
      change_mixer_position(MIXER_DOWN);
      start_mixer(); 
    } else {
      next_state = sPUMPING;
    }
    break;

  case sMIXING:
    if (vars.mixing == false){ //this variable was changed by the ISR
      next_state = sREADY_TO_MAKE;
      change_mixer_position(MIXER_UP);
    } else {
      next_state = sMIXING;
    }
    break;
  case sALL_STOP:
    if (vars.stopped == false){
      displayGreenLED();
      next_state = sREADY_TO_MAKE;
    } else {
      next_state = sALL_STOP;
      
    }
    break;
  }
  if (cur_state != next_state) {
    Serial.println(s2str(next_state));
  }
  return next_state;
}

/*        
 * Helper function for printing states
 */
char* s2str(state s) {
  switch(s) {
    case sSETUP:
    return "(1) Setup";
    case sREADY_TO_MAKE:
    return "(2) Ready";
    case sPUMPING:
    return "(3) Pumping";
    case sMIXER_LOWERING:
    return "(4) Mixer Lowering";
    case sMIXING:
    return "(5) Mixing";
    case sMIXER_RAISING:
    return "(6) Mixer Raising";
    case sALL_STOP:
    return "(7) All Stop";
    default:
    return "???";
  }
}
