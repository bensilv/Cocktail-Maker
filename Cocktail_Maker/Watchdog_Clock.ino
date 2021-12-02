volatile int running_pump_pin = -1;

void setup_clock_watchdog() {
  
}

void start_pump_clock(int pumpPort, int dur_millis) {
  
}

void petWatchdog() {
  // write 0xA5 to clear register to restart watchdog timer
  WDT->CLEAR.reg = 165;
}

void TC3_Handler() {
  
}

void WDT_Handler() {
  
}
