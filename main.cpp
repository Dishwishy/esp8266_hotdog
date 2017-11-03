#include <Adafruit_NeoPixel.h>
#include <ESP8266WiFi.h>          //https://github.com/esp8266/Arduino
#include <ESP8266mDNS.h>
#include <WiFiClient.h>
//needed for library
#include <ESP8266WebServer.h>
#include <DNSServer.h>
#include <WiFiManager.h>          //https://github.com/tzapu/WiFiManager
#include <color.h>

//function prottype
uint32_t Wheel(byte WheelPos);
void rainbowCycle(uint8_t wait);
void stars();

// select wich pin will trigger the configuraton portal when set to LOW
// ESP-01 users please note: the only pins available (0 and 2), are shared 
// with the bootloader, so always set them HIGH at power-up
#define TRIGGER_PIN 0
//neopixel pin
#define NEOPIN 14
//number of pixels
#define PIXNUM 50
//light switch


//set up server
ESP8266WebServer server(80);
//consts to check for state
IPAddress portalIP(192,168,4,1);
IPAddress nullIP(0,0,0,0);
//setup neopixels
Adafruit_NeoPixel strip = Adafruit_NeoPixel(PIXNUM, NEOPIN, NEO_GRB + NEO_KHZ800);
//color storage
int colorInfo [3] = {0,0,0}; 
//counter for setup init
volatile int counter = 0;
int wait = 50;


//URL handlers
void handleRoot() {
  counter = 0;
  server.send(200, "text/html", pageString);
}

void handleLight() {

  colorInfo[0] = server.arg("r").toInt();
  colorInfo[1] = server.arg("g").toInt();
  colorInfo[2] = server.arg("b").toInt();
  for(int i=0;i<PIXNUM;i++){
  strip.setPixelColor(i, server.arg("r").toInt(), server.arg("g").toInt(), server.arg("b").toInt());
  };
  strip.show();  
  server.send(200, "text/text", "success" );
  counter = 0;
  
}

void handleBg() {
  
  String colorData [3];
  for(int i=0;i<3;i++){
    colorData[i] = colorInfo[i];
  }
  server.send(200, "application/json", "{\"r\": " + colorData[0] + ", \"g\": " + colorData[1] + ", \"b\": " + colorData[2] + " }");
}

void handleRnd(){

  stars();
  server.send(200, "text/text", "success" );

}

void handleOff(){
  for(int i=0;i<PIXNUM;i++){
    strip.setPixelColor(i, 0,0,0);
    };
    strip.show();  
    server.send(200, "text/text", "success" );

}

void handleRain(){
  rainbowCycle(20);
  server.send(200, "text/text", "success" );

}


void handleNotFound(){

  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET)?"GET":"POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i=0; i<server.args(); i++){
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);

}


//gets called when WiFiManager enters configuration mode
void configModeCallback (WiFiManager *myWiFiManager) {
  Serial.println("Entered config mode");
  Serial.println(WiFi.softAPIP());
  //if you used auto generated SSID, print it
  Serial.println(myWiFiManager->getConfigPortalSSID());

};




void setup() {
  Serial.begin(115200);
  Serial.println("Starting Up");
//neopixel setup
 strip.begin();
 strip.show(); // Initialize all pixels to 'off'


//Server Setup

  //WiFi.hostname("hotdog");
  WiFiManager wifiManager;
  //wifiManager.resetSettings();
  wifiManager.setAPCallback(configModeCallback);
  wifiManager.setTimeout(180);
  

  if (!wifiManager.autoConnect("hotdog")) {
    Serial.println("failed to connect and hit timeout");
    //reset and try again, or maybe put it to deep sleep
    ESP.reset();
    delay(1000);
  }



  
  //if you get here you have connected to the WiFi
  Serial.println("connected...yeey :)");





    if (MDNS.begin("hotdog")) {
        Serial.println("MDNS responder started");
    }
    server.on("/", handleRoot);
  
    server.on("/setlite", handleLight);

    server.on("/getlite", handleBg);
  
    server.on("/stars", handleRnd);
    server.on("/rnbw", handleRain);
    server.on("/off", handleOff);


    server.onNotFound(handleNotFound);
    
  
    Serial.println("");
    Serial.print("Starting Standard Server ");
  
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
    server.begin();
    MDNS.addService("http", "tcp", 80);
    counter = 0;
    return;
    

    

}


void loop() {
  



  server.handleClient();


  


  
}


uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if(WheelPos < 85) {
   return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  } else if(WheelPos < 170) {
    WheelPos -= 85;
   return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  } else {
   WheelPos -= 170;
   return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
  }
}

void stars(){
  long star = random(49);
  for(int i=0;i<250;i++){
    strip.setPixelColor(star,i,i,i);
    strip.show();  
    delay(5);
  }
  for(int i=250;i>=0;i--){
    strip.setPixelColor(star,i,i,i);
    strip.show();  
    delay(5);
  }
}

// Slightly different, this makes the rainbow equally distributed throughout
void rainbowCycle(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256*5; j++) { // 5 cycles of all colors on wheel
    for(i=0; i< strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel(((i * 256 / strip.numPixels()) + j) & 255));
    }
    strip.show();
    delay(wait);
  }
}


/**
void counterRt(){
  static unsigned long last_interrupt_time = 0;
  unsigned long interrupt_time = millis();
 // If interrupts come faster than 200ms, assume it's a bounce and ignore
 if (interrupt_time - last_interrupt_time > 200) 
 {
   counter ++;
 }
 last_interrupt_time = interrupt_time;
}
**/

