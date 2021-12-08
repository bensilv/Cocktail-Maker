//UNCOMMENT AND FILL IN TO CONNECT TO WIFI
//char ssid[] = "NETWORK NAME";  // network SSID (name)
//char pass[] = "NETWORK PASSWORD"; // for networks that require a password

//char ssid[] = "Sigma Basement";  // network SSID (name)
//char pass[] = "257basement"; // for networks that require a password
//char ssid[] = "Brown-Guest";  // network SSID (name)
//char pass[] = ""; // for networks that require a password
//char ssid[] = "Fios-Tr7ML";  // network SSID (name)
//char pass[] = "dia393law47race"; // for networks that require a password
char ssid[] = "Dumplings";  // network SSID (name)
char pass[] = "dicksonthewall"; // for networks that require a password
int status = WL_IDLE_STATUS;


char heroku[] = "arduino-cocktail-maker.herokuapp.com";
String indexPage = "";

WiFiServer server(80);
Application app;



void setResHeaders(Response &res){
  res.status(200);
  res.set("Content-Type", "application/json");
  res.set("Access-Control-Allow-Origin", "*");
  res.println();
}

void setResBody(Request &req, Response &res, StaticJsonDocument<1000> retDoc){
  serializeJsonPretty(retDoc, *req.stream()); 
  res.println();
  res.flush();
}

StaticJsonDocument<1000> createPostResponse(String success, String err){
  StaticJsonDocument<1000> retDoc;
  deserializeJson(retDoc, "{\"success\":" + success + ",\"error\":\" " + err + "\"}");
  return retDoc;
}


/*
 * Sends the index page to the client
 */
void index(Request &req, Response &res) {
  res.status(200);
  res.set("Content-type", "text/html");
  res.println();
  res.println(indexPage);
  res.println();
  res.flush();
}

/*
 * Get Requests
 */

void getDrinks(Request &req, Response &res) {
  setResHeaders(res);
  setResBody(req, res, getDrinkData());
}

void getPumps(Request &req, Response &res) {
  setResHeaders(res);
  setResBody(req, res, getPumpData());
}

void getStatus(Request &req, Response &res) {
  setResHeaders(res);
  StaticJsonDocument<200> doc;
  doc["status"] = s2str(CURR_STATE);
  setResBody(req, res, doc);
}

/*
 * Post Requests
 */

void postDrinks (Request &req, Response &res) {
  setResHeaders(res);
  StaticJsonDocument<1000> doc;
  deserializeJson(doc, *req.stream());
  updateDrinkData(doc);
  setResBody(req, res, createPostResponse("true", ""));
}


void postPumps (Request &req, Response &res) {
  setResHeaders(res);
  StaticJsonDocument<1000> doc;
  deserializeJson(doc, *req.stream());
  updatePumpData(doc);
  setResBody(req, res, createPostResponse("true", ""));
}


void handleMakeDrink(Request &req, Response &res) {
  setResHeaders(res);
  if (CURR_STATE != sREADY_TO_MAKE) {
    setResBody(req, res, createPostResponse("false", "The machine is currently busy"));
    return;
  }
  
  StaticJsonDocument<500> drinkdoc;
  deserializeJson(drinkdoc, *req.stream());
  JsonObject drink_recipe = drinkdoc["recipe"].as<JsonObject>();

  boolean ok = verifyRequestedRecipe(drink_recipe);

  if (!ok) {
    setResBody(req, res, createPostResponse("false", "Current pumps do not contain all necessary ingredients"));
    return;
  }
  setResBody(req, res, createPostResponse("true", ""));
  makeRequestedRecipe(drink_recipe);
}

void serverSetup() {
  while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to: ");
    Serial.println(ssid);
    status = WiFi.begin(ssid, pass);
    delay(1000);
  }
  Serial.println("Connected!");
  getIndexPage();
  
  IPAddress ip = WiFi.localIP();
  char buffer[40];
  sprintf(buffer, "Server is at: %d.%d.%d.%d", ip[0], ip[1], ip[2], ip[3]);
  Serial.println(buffer);

  app.get("/", &index);
  app.get("/drinks", &getDrinks);
  app.get("/pumps", &getPumps);
  app.get("/status", &getStatus);
  app.post("/drinks", &postDrinks);
  app.post("/pumps", &postPumps);
  app.post("/make-drink", &handleMakeDrink);
  server.begin();
  vars.server_running = true;
}

void handleClient(){
  WiFiClient client = server.available();
  if (client) {
    if (client.connected()) {
      app.process(&client);
      client.stop();
    }
  } 
}



void makeRequestedRecipe(JsonObject drink_recipe){
  ingredient ingredients[drink_recipe.size()];
  int counter = 0;
  for (JsonPair kv : drink_recipe) {
    String liquid = String(kv.key().c_str());
    float amount = kv.value().as<String>().toFloat();
    int pump_num = getPumpNum(liquid);
    ingredients[counter] = ingredient{pump_num, amount};
    counter++;
  }
  recipe r = {
    ingredients,
    counter,
  };

  vars.curr_recipe = r;
  vars.recipe_loaded = true;
}



boolean verifyRequestedRecipe(JsonObject drink_recipe){
  for (JsonPair kv : drink_recipe) {
    String liquid = String(kv.key().c_str());
    float amount = kv.value().as<String>().toFloat();
    int pump_num = getPumpNum(liquid);
    if (pump_num < 0 or amount == 0) {
      return false;
    }
  }
    
   return true;
   
}


int getPumpNum (String ingredient){
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
