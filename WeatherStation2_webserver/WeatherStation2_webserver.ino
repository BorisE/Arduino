/*
  WEATHER STATION Mk II
  (c) 2020 by Boris Emchenko

 Changes:
   ver 0.3 2020/07/19 - onewire implemented
                      - webpages templates changed
   ver 0.2 2020/07/19 - i2c and bme280 implemented
   ver 0.1 2020/07/18 - web data sending implemented (as prototype)
                      - web server implemented 
                      - DHT22 implemented
*/

//Compile version
#define VERSION "0.3"
#define VERSION_DATE "20200719"


#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

#include <Wire.h>
#include <BME280_I2C.h>
#include <OneWire.h>

#ifndef STASSID
#define STASSID "BATMAJ"
#define STAPSK  "8oknehcmE"
#endif

const char* ssid = STASSID;
const char* password = STAPSK;

ESP8266WebServer server(80);

const char* host = "192.168.1.1";  // IP serveur - Server IP
const int   port = 80;            // Port serveur - Server Port
unsigned long _last_HTTP_SEND=0;

/* for Wemos D1 R1
#define PIN_WIRE_SDA (4)  D14
#define PIN_WIRE_SCL (5)  D15

//new ESP-12E GPIO2
#define LED_BUILTIN 2       D9

static const uint8_t D0   = 3;
static const uint8_t D1   = 1;
static const uint8_t D2   = 16;
static const uint8_t D3   = 5;
static const uint8_t D4   = 4;
static const uint8_t D5   = 14;
static const uint8_t D6   = 12;
static const uint8_t D7   = 13; /не заработал OneWire
static const uint8_t D8   = 0;  /startup pin.  pulled up to Vcc. Don't use as intput. Special care as output
static const uint8_t D9   = 2;  /startup pin. LED.  pulled up to Vcc. Don't use as intput. Special care as output
static const uint8_t D10  = 15; /startup pin. pulled down to GND. Don't use as intput. Special care as output
static const uint8_t D11  = 13;
static const uint8_t D12  = 12;
static const uint8_t D13  = 14;
static const uint8_t D14  = 4;
static const uint8_t D15  = 5;

GPIO6-GPIO11 - flash pins

*/

/* NODEMCU
#define PIN_WIRE_SDA (4)    D2
#define PIN_WIRE_SCL (5)    D1

#define LED_BUILTIN 2       D4
#define LED_BUILTIN_AUX 16  D0

static const uint8_t D0   = 16;
static const uint8_t D1   = 5;
static const uint8_t D2   = 4;
static const uint8_t D3   = 0;
static const uint8_t D4   = 2;
static const uint8_t D5   = 14;
static const uint8_t D6   = 12;
static const uint8_t D7   = 13;
static const uint8_t D8   = 15;
static const uint8_t D9   = 3;
static const uint8_t D10  = 1;
*/
const int STATUS_LED = LED_BUILTIN;

#define DHT_PIN D11
enum {DHT22_SAMPLE, DHT_TEMPERATURE, DHT_HUMIDITY, DHT_DATAPTR};  // DHT functions enumerated
enum {DHT_OK = 0, DHT_ERROR_TIMEOUT = -1, DHT_ERROR_CRC = -2, DHT_ERROR_UNKNOWN = -3};  // DHT error codes enumerated
float dhtTemp = -100;
float dhtHum =0;
unsigned long _lastReadTime_DHT=0;


// Create BME280 object
#define SDA_pin D3
#define SCK_pin D4
#define MY_BME280_ADDRESS (0x76)
#define SEALEVELPRESSURE_HPA (1013.25)
BME280_I2C bme(MY_BME280_ADDRESS);
float bmePres = 0;
float bmeTemp = -100;
float bmeHum = 0;
unsigned long _lastReadTime_BME=0;


#define ONE_WIRE_BUS D6 // Data wire is plugged into this port 
OneWire  ds(ONE_WIRE_BUS);  // on pin 10 (a 4.7K resistor is necessary)
//ROM = 28 6D A3 68 4 0 0 F8
uint8_t OW_Temp1Addr[8] = { 0x28, 0x6D, 0xA3, 0x68, 0x4, 0x0, 0x0, 0xF8 };
float OW_Temp1=-100;
unsigned long _lastReadTime_OW=0;


unsigned long currenttime;              // millis from script start 
#define DHT_READ_INTERVAL 10000
#define BME_READ_INTERVAL 10000
#define POST_DATA_INTERVAL 10000
#define OW_READ_INTERVAL 10000

bool bOutput=false;

/********************************************************
*     SETUP
 *******************************************************/
void setup(void) {
  pinMode(STATUS_LED, OUTPUT);
  digitalWrite(STATUS_LED, LOW);
  Serial.begin(115200);
  
  // Wait for serial to initialize.
  while(!Serial) { }
  Serial.println("");

  Serial.println("WEATHER STATION Mk II");
  Serial.print ("v");
  Serial.print (VERSION);
  Serial.print (" [");
  Serial.print (VERSION_DATE);
  Serial.println ("]");

  
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

  if (MDNS.begin("esp8266")) {
    Serial.println("MDNS responder started");
  }

  server.on("/", handleRoot);
  server.on("/temp", handleGetTemp);  

  server.on("/inline", []() {
    server.send(200, "text/plain", "this works as well");
  });
  server.on("/gif", handleGIF);

  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("HTTP server started");


  //init BME280 sensor
  if (!bme.begin(SDA_pin, SCK_pin)) 
  {
    Serial.println("Could not find a valid BME280 sensor, check wiring!");
  } 

  
}

/********************************************************
*     LOOP
 *******************************************************/
void loop(void) {
  bOutput=false;
  currenttime = millis();
  
  server.handleClient();
  MDNS.update();
  
  // Send data to webserver
  // Every given interval
  if ( currenttime - _last_HTTP_SEND > POST_DATA_INTERVAL ) {
      _last_HTTP_SEND = currenttime;
      HTTP_SendData();
  }

  //DHT Read very time consuming
  //So read only in given interval
  if (_lastReadTime_DHT ==0 || (currenttime - _lastReadTime_DHT) > DHT_READ_INTERVAL)
  {
    bOutput=true;
    readDHTSensor(dhtTemp, dhtHum);
    _lastReadTime_DHT= currenttime;
  }
  
  if (_lastReadTime_BME ==0 || (currenttime - _lastReadTime_BME) > BME_READ_INTERVAL)
  {
    bOutput=true;
    getBMEvalues(bmePres, bmeTemp, bmeHum);
    _lastReadTime_BME= currenttime;
  }

  if (_lastReadTime_OW ==0 || (currenttime - _lastReadTime_OW) > OW_READ_INTERVAL)
  {
    bOutput=true;
    OW_Temp1 = getOneWireTemp(OW_Temp1Addr);
    Serial.print("[!OW1:");
    Serial.print(OW_Temp1);
    Serial.println("]");

    _lastReadTime_OW= currenttime;
  }

  if (bOutput)
  {
    Serial.println(F("[!END]\r\n"));
  }

  //delay(5000);
}
