#include <WiFi101.h>
#include <aWOT.h>

//char ssid[] = "Sigma Basement";  // network SSID (name)
//char pass[] = "257basement"; // for networks that require a password
char ssid[] = "Brown-Guest";  // network SSID (name)
char pass[] = ""; // for networks that require a password
int status = WL_IDLE_STATUS;
  
WiFiServer server(80);
Application app;
  
void index(Request &req, Response &res) {
  res.status(200);
  res.set("Content-type", "text/html");
  res.println();
  res.print("Hello World!");
  res.println();
  res.flush();
  //client.stop();
}

void fav(Request &req, Response &res) {
  Serial.println("favicon handler");
}

void react(Request &req, Response &res) {
  res.status(200);
  res.set("Content-type", "text/html");
  res.println();
  res.println(
    "<!DOCTYPE html>\n"
    "<html lang='en'>\n"
      "<head>\n"
        "<script>\n"
          "function openTest() {\n"
            "window.open('http://' + window.location.hostname + '/test', '_self', false)\n"
          "}\n"
        "</script>\n"
        "<base href='https://intense-development-app.herokuapp.com/'/>"
        "<meta charset='utf-8' />\n"
        "<link rel='icon' href='/favicon.ico' />\n"
        "<meta name='viewport' content='width=device-width, initial-scale=1' />\n"
        "<meta name='theme-color' content='#000000' />\n"
        "<meta name='description' content='Web site created using create-react-app' />\n"
        "<link rel='apple-touch-icon' href='/logo192.png' />\n"
        "<link rel='manifest' href='/manifest.json' />\n"
        "<title>Camera Store</title>\n"
      "</head>\n"
      "<body>\n"
        "<button type='button' onclick='openTest()'>Test</button>\n"        
        "<noscript>You need to enable JavaScript to run this app.</noscript>\n"
        "<div id='root'></div>\n"
      "<script src='/static/js/bundle.js'></script><script src='/static/js/0.chunk.js'></script><script src='/static/js/main.chunk.js'></script></body>\n"
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
  
void setup() {
  Serial.begin(9600);
  while(!Serial);
  while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to: ");
    Serial.println(ssid);
    status = WiFi.begin(ssid); // WiFi.begin(ssid, pass) for password
    delay(1000);
  }
  Serial.println("Connected!");
  IPAddress ip = WiFi.localIP();
  char buffer[40];
  sprintf(buffer, "Server is at: %d.%d.%d.%d", ip[0], ip[1], ip[2], ip[3]);
  Serial.println(buffer);

  app.get("/", &index);
  app.get("/react", &react);
  app.get("/test", &test);
  app.get("/favicon.ico", &fav);
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
    
