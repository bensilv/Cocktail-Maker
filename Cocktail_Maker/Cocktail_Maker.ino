#include "Cocktail_Maker.h"

char ssid[] = "Sigma Basement";  // network SSID (name)
char pass[] = "257basement"; // for networks that require a password
//char ssid[] = "Brown-Guest";  // network SSID (name)
//char pass[] = ""; // for networks that require a password
//char ssid[] = "Dumplings";  // network SSID (name)
//char pass[] = "dicksonthewall"; // for networks that require a password
int status = WL_IDLE_STATUS;

char heroku[] = "arduino-cocktail-maker.herokuapp.com";
String indexPage = "";

//
//typedef struct ingredient_t{
//  String liquid;
//  float amount;
//};
//
//typedef struct drink_t{
//  String name;
//  String description;
//  //ingredient_t recipe[];
//};
  
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
    Serial.println("postPumps");
    updatePumpData(doc);
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

void makeDrink(Request &req, Response &res) {
  Serial.println("make drink request");
  
  //setup response stuff
  res.status(200);
  res.set("Content-Type", "application/json");
  res.println();
  StaticJsonDocument<200> retDoc;
  
  //check if machine is busy. need to write this 
  boolean busy = false;
  if (busy){
    deserializeJson(retDoc, "{\"success\":false,\"error\":\"The machine is currently busy\"}");
    serializeJsonPretty(retDoc, *req.stream());
    res.println();
    res.flush();
  } 
  
  //get recipe data 
  StaticJsonDocument<500> drinkdoc;
  deserializeJson(drinkdoc, *req.stream());
  JsonObject recipe = drinkdoc["recipe"].as<JsonObject>();
  
  //get pump data
  String pumps = "";
  serializeJson(getPumpData(), pumps);
  
  //check if we can make the drink
  boolean ok = true;
  for (JsonPair kv : recipe) {
    String liquid = String(kv.key().c_str());
    if (pumps.indexOf(liquid) < 0) {
      ok = false;
      break;
    }
    Serial.println(liquid);
    String amount = kv.value().as<String>();
    Serial.println(amount);
  }
  if (not ok){
    deserializeJson(retDoc, "{\"success\":false,\"error\":\"Current pumps do not contain all necessary ingredients\"}");
  } else{
    deserializeJson(retDoc, "{\"success\":true,\"error\":\"\"}");
    //send drink to drink machine
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
  

//  char json_drink[] = "[{\"name\":\"martini\",\"description\":\"it's a martini\"}]";
//  drink_t drink = parseJSONdrinks(json_drink, 1);
    
                     
  Serial.begin(9600);
  while(!Serial);
//  Serial.println(SDCARD_SS_PIN);
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
}

  
void loop() {  
  WiFiClient client = server.available();
  if (client) {
    
     if (client.connected()) {
      Serial.println("new client");
      app.process(&client);
      Serial.println("client handled");
      client.stop();
      
    }
  }
}
    
