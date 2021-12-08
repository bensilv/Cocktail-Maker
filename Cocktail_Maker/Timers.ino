int CLOCKFREQ = 32768;

void setupTimers() {

  /*
   * General Timer Setup
   */

  // Configure and enable GCLK4 for TC:
  
  GCLK->GENDIV.reg = GCLK_GENDIV_DIV(0) | GCLK_GENDIV_ID(4); 
  while(GCLK->STATUS.bit.SYNCBUSY);
  GCLK->GENCTRL.reg = GCLK_GENCTRL_GENEN | GCLK_GENCTRL_SRC_OSC32K | GCLK_GENCTRL_ID(2) | GCLK_GENCTRL_IDC;
  while(GCLK->STATUS.bit.SYNCBUSY);
  GCLK->CLKCTRL.reg = GCLK_CLKCTRL_CLKEN | GCLK_CLKCTRL_GEN(2) | GCLK_CLKCTRL_ID(0x1B);

  // Configure and enable TC:
  TC3->COUNT16.CTRLA.reg &= ~TC_CTRLA_ENABLE;
  while(TC3->COUNT16.STATUS.bit.SYNCBUSY);
  TC3->COUNT16.INTENCLR.reg |= TC_INTENCLR_MC0;
  
  // Set up NVIC:
  NVIC_SetPriority(TC3_IRQn, 0);
  NVIC_EnableIRQ(TC3_IRQn);

  

  /*
   * Watchdog Setup
   */
  // Clear and enable WDT
  NVIC_DisableIRQ(WDT_IRQn);
  NVIC_ClearPendingIRQ(WDT_IRQn);
  NVIC_SetPriority(WDT_IRQn, 0);
  NVIC_EnableIRQ(WDT_IRQn); 

  // Configure and enable WDT GCLK:
  GCLK->GENDIV.reg = GCLK_GENDIV_DIV(4) | GCLK_GENDIV_ID(5);
  while (GCLK->STATUS.bit.SYNCBUSY);
  //STEP 9: 
  GCLK ->GENCTRL.reg = GCLK_GENCTRL_GENEN | GCLK_GENCTRL_SRC(3) | GCLK_GENCTRL_ID(5) | GCLK_GENCTRL_DIVSEL;
  while (GCLK->STATUS.bit.SYNCBUSY);
  GCLK -> CLKCTRL.reg = GCLK_CLKCTRL_CLKEN | GCLK_CLKCTRL_GEN(5) | GCLK_CLKCTRL_ID(3);
  while (GCLK->STATUS.bit.SYNCBUSY);

  // Configure and enable WDT:
  // WDT->CONFIG.reg, WDT->EWCTRL.reg, WDT->CTRL.reg
  // PER 0x9 (reset every 4096 clock cycles = 4 seconds)
  WDT->CONFIG.reg = WDT_CONFIG_PER(11);
  // EWOFFSET: 0x8 (early warning happens every 2048 cycles = 2 seconds)
  WDT->EWCTRL.reg = WDT_EWCTRL_EWOFFSET(10);
  WDT->CTRL.reg = WDT_CTRL_ENABLE; // enables watchdog.
  while (WDT->STATUS.bit.SYNCBUSY);
  
  // Enable early warning interrupts on WDT:
  // reference WDT registers with WDT->register_name.reg
  WDT->INTENSET.reg = WDT_INTENSET_EW;

  Serial.println("Timers Initialized!");
}

void petWatchdog() {
  // write 0xA5 to clear register to restart watchdog timer
  WDT->CLEAR.reg = 165;
}

void WDT_Handler() {
  // Clear interrupt register flag
  // (reference register with WDT->register_name.reg)
  WDT->INTFLAG.reg = WDT_INTFLAG_EW;
  // Warn user that a watchdog reset may happen
  Serial.println("Watchog Reset Warning");
}

void start_timer(int dur_millis) {
  int duration =  ((32768 * ((float)dur_millis/ (float) 1000)) /64);
  TC3->COUNT16.INTENCLR.reg |= TC_INTENCLR_MC0;
  TC3->COUNT16.CTRLA.reg |= TC_CTRLA_ENABLE | TC_CTRLA_PRESCALER_DIV64 | TC_CTRLA_WAVEGEN_MFRQ | TC_CTRLA_MODE_COUNT16 | TC_CTRLA_PRESCSYNC_PRESC;
  while(TC3->COUNT16.STATUS.bit.SYNCBUSY);
  TC3->COUNT16.CC[0].reg = TC_COUNT16_CC_CC(duration);
  while(TC3->COUNT16.STATUS.bit.SYNCBUSY);
  TC3->COUNT16.INTENSET.reg |= TC_INTENSET_MC0;
}


void TC3_Handler() {
  // Clear interrupt register flag
  TC3->COUNT16.INTFLAG.reg = TC_INTFLAG_MC0;
  TC3->COUNT16.INTENCLR.reg = TC_INTENCLR_MC0;
  TC3->COUNT16.CTRLA.reg &= ~TC_CTRLA_ENABLE;
  
  (*callback)();
}
