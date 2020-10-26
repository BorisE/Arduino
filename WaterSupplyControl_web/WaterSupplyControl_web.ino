/*
  WATER SUPPLY CONTROL
  (c) 2020 by Boris Emchenko

  TODO:
  - WiFi manager
  - Config saving in SPIFFS
  - more asbtracted waterflow control
  - AUTOMATION

   ver 0.8 2020/10/26 [345684/31724] - Controlling - auto switch on, autoswitch off VENT1 based on WS1 sensor
                                     - some JS fix
   ver 0.7 2020/10/26 [344408/29384] - Switch timeout, some small improvements
   ver 0.6 2020/10/23 [343236/29368] - OTA added
   ver 0.5 2020/08/16 [322776/28536] - fully working device with: 2 relays, 3 water level sensors and waterflow sensor
   ver 0.4 2020/08/16                - waterflow sensor counting
   ver 0.3 2020/08/16                - relay/wsensors status to serial output
   ver 0.2 2020/08/16 [320304/28124] - 3 water sensor added
   ver 0.1 2020/08/16 [319288/27988] - relays reading status and changing status throug web page
*/
//Compile version
#define VERSION "0.8a"
#define VERSION_DATE "20201026"

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoOTA.h>
#include <ESP8266HTTPUpdateServer.h>

#ifndef STASSID
#define STASSID "BATMAJ"
#define STAPSK  "8oknehcmE"
#endif

const char* ssid = STASSID;
const char* password = STAPSK;
const char* host = "watersupply";
#define OTA_PORT 18266

ESP8266WebServer server(80);
ESP8266HTTPUpdateServer httpUpdater;


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

#define WS1_PIN_DEFAULT D2  //верхний
#define WS2_PIN_DEFAULT D1  //средний
#define WS3_PIN_DEFAULT D8  //нижний


#define WATERFLOW_PIN_DEFAULT D3

struct Config {
  uint8_t H1_1_PIN;
  uint8_t H1_2_PIN;
  uint8_t H2_1_PIN;
  uint8_t H2_2_PIN;
  uint8_t WS1_PIN;
  uint8_t WS2_PIN;
  uint8_t WS3_PIN;
  uint8_t WATERFLOW_PIN;  
};
Config config;      

volatile int flow_count; // variable to store the “rise ups” from the flowmeter pulses
unsigned int flow_l_min; // Calculated litres/min
#define WATERFLOW_COUNT_FREQUENCY 1000    //millis

#define RELAY_READ_INTERVAL     10000
#define WSENSORS_READ_INTERVAL  10000
#define JS_UPDATEDATA_INTERVAL  10000

#define VENT_CHANGESTATE_TIMEOUT  10000   // how long to give vent to change it state before switching off 
                                          // need to be set experimentally

#define VENT_CLOSE_DELAY        10000     // Delay after WaterSensor stop becomes 1 and starting closing routine
                                          // for debug, better set to 60000 or larger

#define VENT1_CLOSE_RELAY_NAME  "relay2"
#define VENT1_OPEN_RELAY_NAME   "relay1"

//Interrupt function, so that the counting of pulse “rise ups” dont interfere with the rest of the code  (attachInterrupt)
void ICACHE_RAM_ATTR flow_ISR(){   
  flow_count++;
}

char debugstack[2048]; //buffer to store debug messages befor outputing to web

unsigned long currenttime=0;              // millis from script start 
unsigned long _lastReadTime_Relay=0;
unsigned long _lastReadTime_WS=0;
unsigned long _lastReadTime_WaterFlow=0;
uint8_t  WS_top_flag, lastWS_top_flag;//ИНИЦИАЛИЗИРОВАТЬ!
bool needToClose_flag = false,needToOpen_flag = false;
uint8_t  Vent1_ChangingState = 0;
unsigned long waitToEngageVent1_starttime=0;
unsigned long Vent1_ChangeState_starttime=0;

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

  httpUpdater.setup(&server, "/update");

  server.begin();
  Serial.println(F("HTTP server started"));

  ////////////////////////////////
  // OTA Update
  ArduinoOTA.setHostname(host);
  ArduinoOTA.setPort(OTA_PORT);
  //ArduinoOTA.setPassword("esp8266");
  ArduinoOTA.onStart(onStartOTA);
  ArduinoOTA.onEnd([]() {
    Serial.println("\n[OTA] End");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("[OTA] Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError(onErrorOTA);
  ArduinoOTA.begin();
  Serial.println("[OTA] OTA ready");


  ////////////////////////////////
  // START HARDWARE
  initRelays ();

  WS_top_flag = digitalRead(config.WS1_PIN);
  lastWS_top_flag = WS_top_flag;
  
  pinMode(config.WATERFLOW_PIN, INPUT_PULLUP);
  // Attach an interrupt to the ISR vector
  attachInterrupt(digitalPinToInterrupt(config.WATERFLOW_PIN), flow_ISR, RISING);
  sei(); // Enable interrupts  
}

/********************************************************
*     LOOP
********************************************************/
void loop() {
  currenttime = millis();

  server.handleClient();
  MDNS.update();
  ArduinoOTA.handle();
  
   // Every second, calculate and print litres/hour
   if(currenttime >= (_lastReadTime_WaterFlow + WATERFLOW_COUNT_FREQUENCY))
   {
      calcWaterFlow();
      _lastReadTime_WaterFlow = currenttime;
   }

  //Print relay status
  if (_lastReadTime_Relay ==0 || (currenttime - _lastReadTime_Relay) > RELAY_READ_INTERVAL)
  {
    printRelayStatus();
    _lastReadTime_Relay= currenttime;
  }
  //Print relay status
  if (_lastReadTime_WS ==0 || (currenttime - _lastReadTime_WS) > WSENSORS_READ_INTERVAL)
  {
    printWSensorStatus();
    checkSupplyStatus_changeState();
    _lastReadTime_WS= currenttime;
  }
}


//Print debug information
void debug(char* st) {
  
  //Обнулим в случае переполнения
  if ((strlen(debugstack) + strlen(st)) > (sizeof(debugstack)-1)) {
    debugstack[0]='\0';
    strcat(debugstack, "ovf! ");
  }
  strcat(debugstack, st);
  Serial.println(st);
  
}
