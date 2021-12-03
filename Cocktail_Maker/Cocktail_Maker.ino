#include "Cocktail_Maker.h"
#include <Servo.h>
Servo myservo;

state CURR_STATE = sSETUP;
//char ssid[] = "Sigma Basement";  // network SSID (name)
//char pass[] = "257basement"; // for networks that require a password
//char ssid[] = "Brown-Guest";  // network SSID (name)
//char pass[] = ""; // for networks that require a password
//char ssid[] = "Dumplings";  // network SSID (name)
//char pass[] = "dicksonthewall"; // for networks that require a password
char ssid[] = "Fios-Tr7ML";  // network SSID (name)
char pass[] = "dia393law47race"; // for networks that require a password
int status = WL_IDLE_STATUS;

boolean stop_disabled = false;

char heroku[] = "arduino-cocktail-maker.herokuapp.com";
String indexPage = "";
int last_emergency_stop = 0;
num_pumps_finished = 0;


//guard member variables

boolean server_running = false;

state_variables vars = {
  MIXER_UP, //mixer pos
  false, // mising
  NULL, //pointer to recipe
  0, //num pumps
  false //stopped
};





WiFiServer server(80);
Application app;

void fav(Request &req, Response &res) {
  Serial.println("favicon handler");
}

void index(Request &req, Response &res) {
  res.status(200);
  res.set("Content-type", "text/html");
  res.println();
  res.println(indexPage);
  res.println();
  res.flush();
  //client.stop();
}

void test(Request &req, Response &res) {
  Serial.println("test route");
  res.status(200);
  res.set("Content-type", "text/html");
  res.println();
  res.print("Test");
  res.println();
  res.flush();

  //client.stop();
  Serial.println("done");

}

/**
   Get Requests
*/

void getDrinks(Request &req, Response &res) {
  Serial.print("getting drinks");
  res.status(200);
  res.set("Content-type", "application/json");
  res.set("Access-Control-Allow-Origin", "*");
  res.println();
  serializeJsonPretty(getDrinkData(), *req.stream());
  res.println();
  res.flush();
}


void getPumps(Request &req, Response &res) {
  Serial.print("getting pumps");
  res.status(200);
  res.set("Content-type", "application/json");
  res.set("Access-Control-Allow-Origin", "*");
  res.println();
  serializeJsonPretty(getPumpData(), *req.stream());
  res.println();
  res.flush();
}


void getStatus(Request &req, Response &res) {
  Serial.println("getting status");
  res.status(200);
  res.set("Content-type", "application/json");
  res.set("Access-Control-Allow-Origin", "*");
  res.println();
  StaticJsonDocument<200> doc;
  doc["status"] = s2str(CURR_STATE);
  //  Serial.println((int) CURR_STATE);
  serializeJsonPretty(doc, *req.stream());
  res.println();
  res.flush();
}

/**
   Post Requests
*/

void postDrinks (Request &req, Response &res) {
  StaticJsonDocument<1000> doc;
  deserializeJson(doc, *req.stream());
  Serial.println("postDrinks");
  updateDrinkData(doc);
  res.status(200);
  res.set("Content-Type", "application/json");
  res.set("Access-Control-Allow-Origin", "*");
  res.println();
  StaticJsonDocument<200> retDoc;
  deserializeJson(doc, "{\"success\":true,\"error\":\"\"}");
  serializeJsonPretty(doc, *req.stream());
  res.println();
  res.flush();
}

void postPumps (Request &req, Response &res) {
  StaticJsonDocument<1000> doc;
  deserializeJson(doc, *req.stream());
  res.status(200);

  Serial.println("postPumps");
  updatePumpData(doc);

  res.set("Content-Type", "application/json");
  res.set("Access-Control-Allow-Origin", "*");
  res.println();
  StaticJsonDocument<200> retDoc;
  deserializeJson(doc, "{\"success\":true,\"error\":\"\"}");
  serializeJsonPretty(doc, *req.stream());
  res.println();
  res.flush();
}

void makeDrink(Request &req, Response &res) {
  
  //setup response stuff
  res.status(200);
  res.set("Content-Type", "application/json");
  res.println();
  StaticJsonDocument<200> retDoc;

  //check if machine is busy. need to write this

  if (CURR_STATE != sREADY_TO_MAKE) {
    deserializeJson(retDoc, "{\"success\":false,\"error\":\"The machine is currently busy\"}");
    serializeJsonPretty(retDoc, *req.stream());
    res.println();
    res.flush();
    return;
  }

  //get recipe data
  StaticJsonDocument<500> drinkdoc;
  deserializeJson(drinkdoc, *req.stream());
  JsonObject drink_recipe = drinkdoc["recipe"].as<JsonObject>();

  //check if we can make the drink
  boolean ok = true;
  ingredient ingredients[drink_recipe.size()];
  int counter = 0;
  for (JsonPair kv : drink_recipe) {
    String liquid = String(kv.key().c_str());
    String amount = kv.value().as<String>();

    float amount_cast = amount.toFloat(); 

    int pump_num = get_pump_num(liquid);
    if (pump_num < 0 or amount_cast == 0) {
      ok = false;
      break;
    }

    //TODO convert amount string to float
    ingredients[counter] = ingredient{pump_num, amount_cast};
    counter++;
  }

  if (not ok) {
    deserializeJson(retDoc, "{\"success\":false,\"error\":\"Current pumps do not contain all necessary ingredients\"}");
  } else {
    deserializeJson(retDoc, "{\"success\":true,\"error\":\"\"}");
    recipe r = {
      ingredients,
      counter,
    };
    vars.curr_recipe = r;
    vars.recipe_loaded = true;

  }

  serializeJsonPretty(retDoc, *req.stream());
  res.println();
  res.flush();
}

void getIndexPage() {
  Serial.println("Getting Heroku Index Page");
  WiFiClient client;
  if (client.connect(heroku, 80)) {
    client.println("GET / HTTP/1.1");
    client.println("HOST: arduino-cocktail-maker.herokuapp.com");
    client.println("Connection: close");
    client.println();
    bool clientRead = false;
    bool htmlReached = false;
    while (!clientRead) {
      int len = client.available();
      if (len > 0) {
        byte buffer[80];
        if (len > 80) len = 80;
        client.read(buffer, len);
        for (int i = 0; i < len; i++) {
          if (!htmlReached) {
            if ((char) (buffer[i]) == '<') {
              htmlReached = true;
            }
          }
          if (htmlReached) {
            indexPage += (char) buffer[i];
          }
        }
      }

      if (!client.connected()) {
        client.stop();
        clientRead = true;
        Serial.println("Heroku Index Page Loaded");
        //        Serial.println(indexPage);
      }
    }
  } else {
    Serial.println("Loading Heroku Failed");
  }
}

void setup() {
  Serial.begin(9600);

  //mechanical initializations
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
  attachInterrupt(digitalPinToInterrupt(BUTTON), emergency_stop, RISING);

  while (!Serial);

  SDSetup();
  CURR_STATE = sSETUP;
  Serial.println(s2str(CURR_STATE));
  while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to: ");
    Serial.println(ssid);
    status = WiFi.begin(ssid, pass); // WiFi.begin(ssid, pass) for password
    delay(1000);
  }
  Serial.println("Connected!");
  getIndexPage();
  setup_clock_watchdog();
  IPAddress ip = WiFi.localIP();
  char buffer[40];
  sprintf(buffer, "Server is at: %d.%d.%d.%d", ip[0], ip[1], ip[2], ip[3]);
  Serial.println(buffer);

  app.get("/", &index);
  app.get("/test", &test);
  app.get("/favicon.ico", &fav);
  app.get("/drinks", &getDrinks);
  app.get("/pumps", &getPumps);
  app.get("/status", &getStatus);
  app.post("/drinks", &postDrinks);
  app.post("/pumps", &postPumps);
  app.post("/make-drink", &makeDrink);
  server.begin();
  server_running = true;


}


void loop() {
  petWatchdog();
  CURR_STATE = update_fsm(CURR_STATE, server_running, vars);
  WiFiClient client = server.available();
  if (client) {
    if (client.connected()) {
      Serial.println("new client");
      app.process(&client);
      Serial.println("client handled");
      client.stop();
    }
  } else {
    delay(10);
  }
}

/////////////////////MECHANICS///////////////////////

void start_pumps(recipe ordered_recipe) {

  //start each pump, start service routine to stop that pump after x amount of time
  //in the ISR decrement vars.num_pumps_running


  //for now:
  vars.num_pumps_running = ordered_recipe.num_ingredients;
  vars.recipe_loaded = false;

  //start pumps
  for (int i = 0; i < ordered_recipe.num_ingredients; i++) {
    ingredient ing = ordered_recipe.ingredients[i];
    start_pump(ing.pump, ing.amount);
    vars.num_pumps_running--;
  }
}

void change_mixer_position(mixer_position new_pos) {
  if (vars.stopped) return;
  if (new_pos == MIXER_UP) {
    // send mixer up
    myservo.write(40);
    delay(1000);
    vars.mixer_pos = new_pos;
  } else {
    //send mixer down
    myservo.write(120);
    delay(1000);
    vars.mixer_pos = new_pos;
  }
}

void start_mixer() {
  if (vars.stopped) return;
  vars.mixing = true;
  analogWrite(DC_MOTOR, 200);
  delay_helper(1000);
  analogWrite(DC_MOTOR, 0);

  vars.mixing = false;
}



void start_pump(int pump, float amount) {
  if (vars.stopped) return;
  digitalWrite(pump, HIGH);
  //call ounces_to_seconds(amount) and pass to delay
  delay_helper(ounces_to_seconds(amount));
  digitalWrite(pump, LOW);
}

float ounces_to_seconds(float amount) { //note: amount is in ounces  
  return amount*600;
}


void emergency_stop() {
  for(int i=0; i<100; i++){
    if (digitalRead(BUTTON) != 1){
      return;
    }
  }
  
  Serial.println("not noise");
  int curr_time = millis();
  
  if (curr_time - last_emergency_stop < 500 ){
    Serial.print("curr time: ");
    Serial.println(curr_time);
    Serial.print("last time stopped: ");
    Serial.println(last_emergency_stop);
    return;
  }
  last_emergency_stop = curr_time;
  
  
  if (vars.stopped == true) {
    Serial.println("EMERGENCY un-STOP");
    reset_state();
  } else {
    vars.stopped = true;
    Serial.println("EMERGENCY STOP");
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

void reset_state() {
  vars.stopped = false;
  vars.recipe_loaded = false;
  vars.num_pumps_finished = 0;
  vars.mixing = false;
  vars.mixer_pos = MIXER_UP;
  myservo.write(40);
}


void delay_helper(int mils){
  int start_time = millis();
  while(millis() - start_time < mils){
    if (digitalRead(BUTTON) == 1){
      emergency_stop();
    }  
     
  }
}
