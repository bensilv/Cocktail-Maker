


state update_fsm(state cur_state, boolean server_running, 
                state_variables vars) {
                  
  state next_state;
  if (vars.stopped == true){
    next_state = sALL_STOP;
    if (cur_state != next_state) {
      Serial.println(s2str(next_state));
    }
    digitalWrite(RED_LED, HIGH);
    digitalWrite(GREEN_LED, LOW);
    return sALL_STOP;
  }
  
  switch(cur_state) {
  case sSETUP:
    if (server_running){ // 1->2
      next_state = sREADY_TO_MAKE;
    } else {
      next_state = sSETUP;
      digitalWrite(GREEN_LED, HIGH);
      digitalWrite(RED_LED, LOW);
    }
    break;
  case sREADY_TO_MAKE:
    if (vars.recipe_loaded){
      next_state = sSTART_PUMP;
    } else {
      next_state = sREADY_TO_MAKE;
    }
    break;
  case sSTART_PUMP:
    int curr_pump_pin = vars.curr_recipe.ingredients[vars.num_pumps_finished];
    start_pump(curr_pump_pin);
    next_state = sPUMPING;
    break;
  case sPUMPING:
    if (vars.num_pumps_finished == vars.curr_recipe.num_ingredients){
      next_state = sMIXER_LOWERING;
      change_mixer_position(MIXER_DOWN);
    } else if (vars.num_pumps_finished == vars.num_pumps_started){
      next_state = sSTART_PUMP;
    }else {
      next_state = sPUMPING;
    }
    break;
  case sMIXER_LOWERING:
    if (vars.mixer_pos == MIXER_DOWN){
      next_state = sMIXING;
      start_mixer();
    } else {
      next_state = sMIXER_LOWERING;
    }
    break;
  case sMIXING:
    if (vars.mixing == false){
      next_state = sMIXER_RAISING;
      change_mixer_position(MIXER_UP);
    } else {
      next_state = sMIXING;
    }
    break;
  case sMIXER_RAISING: 
    if (vars.mixer_pos == MIXER_UP){
      next_state = sREADY_TO_MAKE;
    } else {
      next_state = sMIXER_RAISING;
    }
    break;
  case sALL_STOP:
    if (vars.stopped == false){
      digitalWrite(GREEN_LED, HIGH);
      digitalWrite(RED_LED, LOW);
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
