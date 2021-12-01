


state update_fsm(state cur_state, boolean server_running, 
                state_variables vars) {
                  
  state next_state;

  if (vars.stopped == true){
    return sALL_STOP;
  }
  
  switch(cur_state) {
  case sSETUP:
    if (server_running){ // 1->2
      next_state = sREADY_TO_MAKE;
    }
    break;
  case sREADY_TO_MAKE:
    if (vars.curr_recipe != NULL){
      start_pumps(vars.curr_recipe);
      next_state = sPUMPING;
    }
    break;
  case sPUMPING:
    if (vars.num_pumps_running == 0){
      next_state = sMIXER_LOWERING;
      change_mixer_position(MIXER_DOWN);
    }
    break;
  case sMIXER_LOWERING:
    if (vars.mixer_pos == MIXER_DOWN){
      next_state = sMIXING;
      start_mixer();
    }
    break;
  case sMIXING:
    if (vars.mixing == false){
      next_state = sMIXER_RAISING;
      change_mixer_position(MIXER_UP);
    }
    break;
  case sMIXER_RAISING: 
    if (vars.mixer_pos == MIXER_UP){
      next_state = sREADY_TO_MAKE;
    }
    break;
  case sALL_STOP:
    break;
  }
  return next_state;
}
