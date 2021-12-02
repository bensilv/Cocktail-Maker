


state update_fsm(state cur_state, boolean server_running, 
                state_variables vars) {
                  
  state next_state;
  if (vars.stopped == true){
    Serial.println("stopping");
    return sALL_STOP;
  }
  
  switch(cur_state) {
  case sSETUP:
    if (server_running){ // 1->2
      next_state = sREADY_TO_MAKE;
    } else {
      next_state = sSETUP;
    }
    break;
  case sREADY_TO_MAKE:
    if (vars.curr_recipe != NULL){
      start_pumps(vars.curr_recipe);
      next_state = sPUMPING;
    } else {
      next_state = sREADY_TO_MAKE;
    }
    break;
  case sPUMPING:
    if (vars.num_pumps_running == 0){
      next_state = sMIXER_LOWERING;
      change_mixer_position(MIXER_DOWN);
    } else {
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
      return sREADY_TO_MAKE;
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
