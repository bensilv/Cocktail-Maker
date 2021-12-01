#include "Cocktail_Maker.h"

state CURR_STATE = sSETUP;
char ssid[] = "Sigma Basement";  // network SSID (name)
char pass[] = "257basement"; // for networks that require a password
//char ssid[] = "Brown-Guest";  // network SSID (name)
//char pass[] = ""; // for networks that require a password
//char ssid[] = "Dumplings";  // network SSID (name)
//char pass[] = "dicksonthewall"; // for networks that require a password
int status = WL_IDLE_STATUS;

char heroku[] = "arduino-cocktail-maker.herokuapp.com";
String indexPage = "";


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
 * Get Requests
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
  deserializeJson(doc, "{\"status\":\"Ready to Make\"}");
  serializeJsonPretty(doc, *req.stream());
  res.println();
  res.flush();
}

/**
 * Post Requests
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
//    for (int i = 0; i < numDrinks; i++){
//      String name = doc[i]["name"];
//      String description = doc[i]["description"];
//      return   
//    }
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
<<<<<<< HEAD
  Serial.println("make drink request");
  
=======
  Serial.print("make drink request");
>>>>>>> c5d9f3660f8d448542535887281eefd9a0d343c1
  //setup response stuff
  res.status(200);
  res.set("Content-Type", "application/json");
  res.println();
  StaticJsonDocument<200> retDoc;
  
  //check if machine is busy. need to write this 

  if (CURR_STATE != sREADY_TO_MAKE){
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
    
    int pump_num = get_pump_num(liquid);
    if (pump_num < 0) {
      ok = false;
      break;
    }
    ingredients[counter] = ingredient{pump_num, 1.1};
     
  }
  if (not ok){
    deserializeJson(retDoc, "{\"success\":false,\"error\":\"Current pumps do not contain all necessary ingredients\"}");
  } else{
    deserializeJson(retDoc, "{\"success\":true,\"error\":\"\"}");
    recipe r = {
      *ingredients,
    };
    vars.curr_recipe = &r;
    
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
  while(!Serial);
  SDSetup();
  while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to: ");
    Serial.println(ssid);
    status = WiFi.begin(ssid, pass); // WiFi.begin(ssid, pass) for password
    delay(1000);
  }
  Serial.println("Connected!");
  getIndexPage();
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
  CURR_STATE = update_fsm(CURR_STATE, server_running, vars);
  
  WiFiClient client = server.available();
  if (client) {
     if (client.connected()) {
      Serial.println("new client");
      app.process(&client);
      Serial.println("client handled");
      client.stop();
    }
  }else{
    delay(10);
  }
}




// mechanical stuff

void start_pumps(recipe *ordered_recipe){

 
  
  //start each pump, start service routine to stop that pump after x amount of time
  //in the ISR decrement vars.num_pumps_running

  //for now:
  vars.num_pumps_running = 4;
  vars.curr_recipe = NULL;
  delay(6000);
  vars.num_pumps_running = 0;
 
}

void change_mixer_position(mixer_position new_pos){
  if (new_pos == MIXER_UP){
    // send mixer up
    vars.mixer_pos = new_pos;
  } else {
    //send mixer down
    vars.mixer_pos = new_pos;
  }
}

void start_mixer(){
  vars.mixing = true;
  delay(5000);
  vars.mixing = false;
  
}



    
