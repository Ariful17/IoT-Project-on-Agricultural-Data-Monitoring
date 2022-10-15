#ifdef ESP32
  #include <WiFi.h>
  #include <ESPAsyncWebServer.h>
  #include <SPIFFS.h>
#else
  #include <Arduino.h>
  #include <ESP8266WiFi.h>
  #include <Hash.h>
  #include <ESPAsyncTCP.h>
  #include <ESPAsyncWebServer.h>
  #include <FS.h>
#endif
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>
#include <Adafruit_ADS1X15.h>

Adafruit_ADS1115 ads;
DHT dht(D3, DHT11);

float b=0.0;
float c=0.0;
float adc0, adc1;


// Replace with your network credentials
const char* ssid = "your wifi name";
const char* password = "your password";

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);
//for fixed IP
//put your router and IP information here to get a fixed IP
IPAddress local_IP(192, 168, 0, 28);
IPAddress gateway(192, 168, 0, 1);
IPAddress subnet(255, 255, 255, 0);

String readrain(){ 
  float a= ((27250-adc1)/27250)*100;
  return String(a);
}
  
String readtemperature() {
  // Read temperature as Celsius (the default)
     float t=b;
    return String(t);
  }
  
String readhumidity() {
    float h=c;
    return String(h);
}
String readmoisture() {
   float p= ((27250-adc0)/27250)*100;
  Serial.println(p);
    return String(p);
}

void setup(){
  // Serial port for debugging purposes
  Serial.begin(115200);
  dht.begin();
  ads.begin();
  
  
  // Initialize SPIFFS
  if(!SPIFFS.begin()){
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }
  //for fixed IP
   if (!WiFi.config(local_IP, gateway, subnet)) {
    Serial.println("STA Failed to configure");
  }
  

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }

  // Print ESP Local IP Address
  Serial.println(WiFi.localIP());

  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/webpage.html");
  });
  server.on("/temperature", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", readtemperature().c_str());
  });
  server.on("/humidity", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", readhumidity().c_str());
  });
    server.on("/moisture", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", readmoisture().c_str());
  });
      server.on("/rain", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", readrain().c_str());
  });


  // Start server
  server.begin();
}
 
void loop(){
  delay(100);
   b=dht.readTemperature();
   delay(100);
   c=dht.readHumidity();
   Serial.println(b);
  adc0 = ads.readADC_SingleEnded(0);
  adc1 = ads.readADC_SingleEnded(1);
}
