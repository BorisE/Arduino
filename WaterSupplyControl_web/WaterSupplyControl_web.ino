/*
  WATER SUPPLY CONTROL
  (c) 2020 by Boris Emchenko

   ver 0.2 2020/08/16 [320304/28124] - 3 water sensor added
   ver 0.1 2020/08/16 [319288/27988] - relays reading status and changing status throug web page
*/
//Compile version
#define VERSION "0.2"
#define VERSION_DATE "20200816"

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <ESP8266HTTPClient.h>

#ifndef STASSID
#define STASSID "BATMAJ"
#define STAPSK  "8oknehcmE"
#endif

const char* ssid = STASSID;
const char* password = STAPSK;
const char* host = "watersupply";

ESP8266WebServer server(80);


/* for Wemos D1 R2
 *  
#define PIN_WIRE_SDA (D1)    4   (by default, but I redefined, see below)
#define PIN_WIRE_SCL (D2)    5   (by default, but I redefined, see below)

//new ESP-12E GPIO2
#define LED_BUILTIN 2       D4

static const uint8_t D0   = 16; /no interrupts                    <-relay
static const uint8_t D1   = 5;  /SCL     
static const uint8_t D2   = 4;  /SDA
static const uint8_t D3   = 0;  /10k pullup                       <-always low on start
static const uint8_t D4   = 2;  /Tx1, 10k pullup, Buit-in LED     <-яркий диод
static const uint8_t D5   = 14; /built in led2 ("SCK")            <-relay
static const uint8_t D6   = 12;                                   <-relay
static const uint8_t D7   = 13; /RX0* 10k pulldown                <-relay
static const uint8_t D8   = 15; /Tx0*                             <-при загузке скетча использует этот пин. потом можно использовать; для реле не подходит

GPIO6-GPIO11 - flash pins
*/
const int STATUS_LED = LED_BUILTIN;

#define H1_1_PIN_DEFAULT D7
#define H1_2_PIN_DEFAULT D6
#define H2_1_PIN_DEFAULT D5 
#define H2_2_PIN_DEFAULT D0

#define WS1_PIN_DEFAULT D1
#define WS2_PIN_DEFAULT D2
#define WS3_PIN_DEFAULT D8 

struct Config {
  uint8_t H1_1_PIN;
  uint8_t H1_2_PIN;
  uint8_t H2_1_PIN;
  uint8_t H2_2_PIN;
  uint8_t WS1_PIN;
  uint8_t WS2_PIN;
  uint8_t WS3_PIN;
};
Config config;      



unsigned long currenttime;              // millis from script start 
#define JS_UPDATEDATA_INTERVAL  10000

/********************************************************
*     SETUP
********************************************************/
void setup() {
  pinMode(STATUS_LED, OUTPUT);
  digitalWrite(STATUS_LED, LOW);

  Serial.begin(115200);
  
  // Wait for serial to initialize.
  while(!Serial) { }
  Serial.println("");

  Serial.println(F("WATER SUPPLY CONTROL"));
  Serial.print ("v");
  Serial.print (VERSION);
  Serial.print (" [");
  Serial.print (VERSION_DATE);
  Serial.println ("]");

  //Load config data
  LoadDefaults();
  //LoadConfigData();

  ////////////////////////////////
  // WiFI managent part
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    digitalWrite(STATUS_LED, HIGH);
    Serial.print(".");
    delay(400);
    digitalWrite(STATUS_LED, LOW);
    delay(100);
  }

  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  ////////////////////////////////
  // MDNS INIT
  if (MDNS.begin(host)) {
    MDNS.addService("http", "tcp", 80);
    Serial.print(F("Open http://"));
    Serial.print(host);
    Serial.println(F(".local to access WaterSupplyControl interface"));
  }

  ////////////////////////////////
  // WebServer INIT
  server.on("/", handleRoot);
  server.on("/json", handleJSON);
  server.on("/ping", handlePingRequest);
  server.on("/relay", handleRelaySwitch);
  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println(F("HTTP server started"));

  ////////////////////////////////
  // START HARDWARE

  //Set relay ping
  initRelays ();
}

/********************************************************
*     LOOP
********************************************************/
void loop() {
  currenttime = millis();

  server.handleClient();
  MDNS.update();

}
