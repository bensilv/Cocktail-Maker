#include "Cocktail_Maker.h"
#include <WiFi101.h>
#include <aWOT.h>
#include <ArduinoJson.h>

//char ssid[] = "Sigma Basement";  // network SSID (name)
//char pass[] = "257basement"; // for networks that require a password
//char ssid[] = "Brown-Guest";  // network SSID (name)
//char pass[] = ""; // for networks that require a password
char ssid[] = "Dumplings";  // network SSID (name)
char pass[] = "dicksonthewall"; // for networks that require a password
int status = WL_IDLE_STATUS;


typedef struct ingredient_t{
  String liquid;
  float amount;
};

typedef struct drink_t{
  String name;
  String description;
  //ingredient_t recipe[];
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
  res.println(
    "<!DOCTYPE html>\n"
    "<html lang='en'>\n"
      "<head>\n"
        "<style type='text/css'>\n"
            "html {\n"
                "overflow: auto;\n"
            "}\n"
            "html,\n"
            "body,\n"
            "div,\n"
            "iframe {\n"
                "margin: 0px;\n"
                "padding: 0px;\n"
                "height: 100%;\n"
                "border: none;\n"
            "}\n"
            "iframe {\n"
                "display: block;\n"
                "width: 100%;\n"
                "border: none;\n"
                "overflow-y: auto;\n"
                "overflow-x: hidden;\n"
            "}\n"
        "</style>\n"
        "<title>Party Time!</title>\n"
      "</head>\n"
      "<body>\n"
        "<iframe src='https://arduino-cocktail-maker.herokuapp.com/'\n"
            "frameborder='0'\n"
            "marginheight='0'\n"
            "marginwidth='0'\n"
            "width='100%'\n"
            "height='100%'\n" 
            "scrolling='auto'></iframe >\n"
      "</body>\n"
    "</html>\n");
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

void getDrinks(Request &req, Response &res) {
  Serial.print("getting drinks");
  res.status(200);
  res.set("Content-type", "application/json");
  res.println();
  serializeJsonPretty(getDrinkData(), *req.stream());
  res.println();
  res.flush();
}
//
//void parseJSONdrinks (char json[], int numDrinks) {
//    StaticJsonDocument<500> doc;
//    DeserializationError error = deserializeJson(doc, json);
//    for (int i = 0; i < numDrinks; i++){
//      String name = doc[i]["name"];
//      String description = doc[i]["description"];
//      return   
//    }
//}
  
void setup() {

//  char json_drink[] = "[{\"name\":\"martini\",\"description\":\"it's a martini\"}]";
//  drink_t drink = parseJSONdrinks(json_drink, 1);
    
                     

  
  Serial.begin(9600);
  while(!Serial);
  while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to: ");
    Serial.println(ssid);
    status = WiFi.begin(ssid, pass); // WiFi.begin(ssid, pass) for password
    delay(1000);
  }
  Serial.println("Connected!");
  IPAddress ip = WiFi.localIP();
  char buffer[40];
  sprintf(buffer, "Server is at: %d.%d.%d.%d", ip[0], ip[1], ip[2], ip[3]);
  Serial.println(buffer);

  app.get("/", &index);
  app.get("/test", &test);
  app.get("/favicon.ico", &fav);
  app.get("/get-drinks", &getDrinks);
  server.begin();
}

//void print_client(WiFiClient client){
//    while (client.available()) {
//      Serial.write(client.read());
//    }
//}


  
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
  
 




  

//  if (client) {
//    client.stop();
//    Serial.println("stopped");
//  }
    
