

int get_pump_num (String ingredient){
  String pumps = "";
  StaticJsonDocument<1000> pumpDoc = getPumpData();
  JsonArray pumpList = pumpDoc.as<JsonArray>();
  int i = 0;
  for (JsonVariant v : pumpList) {
    if (ingredient == v.as<String>()) {
      switch(i) {
        case 0: 
          return PUMP_ONE;
        case 1:
          return PUMP_TWO;
        case 2:
          return PUMP_THREE;
        case 3: 
          return PUMP_FOUR;
        default:
          return -1;
      }
    }
    i++;
  }
  return -1;
}
