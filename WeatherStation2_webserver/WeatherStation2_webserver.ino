/*
  WEATHER STATION Mk II
  (c) 2020 by Boris Emchenko

  TODO:
  - call config page
  - save parameters useing SPIFFS (upload url, maybe other parameters?)
  - data loggins using SPIFFS
  - BH1750
  - Capacitive rain sensor
  - UV sensor?
  - CO2 sensor?
  - OTA web update
  - Deepsleep mode?

 Changes:
   ver 0.9 2020/08/02 [383396/31808] 
                      - Enter config mode from web interface (strange behaviour) 
                      - Some HTML correction
   ver 0.8 2020/08/02 [381360/31752] 
                      - Regular reconnect attempts if WiFi credentials was specified earlier (useful after house power loss startup - WiFi Router need can't load before ESP boot up)
                      - WiFi regular check in loop
   ver 0.7 2020/08/02 [381012/31212] 
                      - WiFiManager lib to configure WiFi connection
   ver 0.6 2020/08/01 [333456/28808] 
                      - moving to DallasTemperature lib because local hangs sensor
   ver 0.5 2020/07/19 [331924/28788] 
                      - json post
                      - bugfixes
   ver 0.4 2020/07/19 - MLX implemented
                      - return json data
                      - webpage js update 
   ver 0.3 2020/07/19 - onewire implemented
                      - webpages templates changed
   ver 0.2 2020/07/19 - i2c and bme280 implemented
   ver 0.1 2020/07/18 - web data sending implemented (as prototype)
                      - web server implemented 
                      - DHT22 implemented
*/

//Compile version
#define VERSION "0.9"
#define VERSION_DATE "20200802"

#include <WiFiManager.h> // https://github.com/tzapu/WiFiManager
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <ESP8266HTTPClient.h>

#include <Wire.h>
#include <BME280_I2C.h>
#include <OneWire.h>
#include <MLX90614.h>
#include <DallasTemperature.h>

//#ifndef STASSID
//#define STASSID "BATMAJ"
//#define STAPSK  ""
//#endif
//const char* ssid = STASSID;
//const char* password = STAPSK;
const char* ssid = "WeatherStation";
const char* host = "weather";

unsigned long _lastWiFi_checkattempt = 0;
#define WIFI_CHECK_CONNECTION_INTERVAL  120000
#define WIFI_CONFIG_PORTAL_WAITTIME  60

ESP8266WebServer server(80);

const char* POST_URL = "http://192.168.0.199/weather/adddata.php"; //Where to post data
unsigned long _last_HTTP_SEND=0;


/* for Wemos D1 R1
 *  
#define PIN_WIRE_SDA (4)  D14   (by default, but I redefined, see below)
#define PIN_WIRE_SCL (5)  D15   (by default, but I redefined, see below)

//new ESP-12E GPIO2
#define LED_BUILTIN 2       D9

static const uint8_t D0   = 3;
static const uint8_t D1   = 1;
static const uint8_t D2   = 16;
static const uint8_t D3   = 5;                --> SDA
static const uint8_t D4   = 4;                --> SCL
static const uint8_t D5   = 14;
static const uint8_t D6   = 12;               --> OneWire bus
static const uint8_t D7   = 13;               /не заработал OneWire
static const uint8_t D8   = 0;  /startup pin.  pulled up to Vcc. Don't use as intput. Special care as output
static const uint8_t D9   = 2;  /startup pin. LED.  pulled up to Vcc. Don't use as intput. Special care as output         -->Used as LED
static const uint8_t D10  = 15; /startup pin. pulled down to GND. Don't use as intput. Special care as output
static const uint8_t D11  = 13;               --> DHT11
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
#define SCL_pin D4
#define MY_BME280_ADDRESS (0x76)
#define SEALEVELPRESSURE_HPA (1013.25)
BME280_I2C bme(MY_BME280_ADDRESS);
float bmePres = 0;
float bmeTemp = -100;
float bmeHum = 0;
unsigned long _lastReadTime_BME=0;


#define ONE_WIRE_BUS_PIN D6 // Data wire is plugged into this port 
OneWire  OneWireBus(ONE_WIRE_BUS_PIN);  // on pin 10 (a 4.7K resistor is necessary)
//ROM = 28 6D A3 68 4 0 0 F8
uint8_t OW_Temp1Addr[8] = { 0x28, 0x6D, 0xA3, 0x68, 0x4, 0x0, 0x0, 0xF8 };
float OW_Temp1=-100;
unsigned long _lastReadTime_OW=0;

DallasTemperature ds18b20(&OneWireBus);

// MLX90614 part
MLX90614 mlx = MLX90614();
float mlxAmb = -100;
float mlxObj = -100;
unsigned long _lastReadTime_MLX=0;


unsigned long currenttime;              // millis from script start 
#define POST_DATA_INTERVAL  120000
#define JS_UPDATEDATA_INTERVAL  10000
#define DHT_READ_INTERVAL   10000
#define BME_READ_INTERVAL   10000
#define OW_READ_INTERVAL    10000
#define MLX_READ_INTERVAL   10000

bool bOutput=false;

/********************************************************
*     SETUP
 *******************************************************/
void setup(void) {
  pinMode(STATUS_LED, OUTPUT);
  digitalWrite(STATUS_LED, LOW);

  Serial.begin(115200);
  //Serial.setDebugOutput(true);  
  
  // Wait for serial to initialize.
  while(!Serial) { }
  Serial.println("");

  Serial.println("WEATHER STATION Mk II");
  Serial.print ("v");
  Serial.print (VERSION);
  Serial.print (" [");
  Serial.print (VERSION_DATE);
  Serial.println ("]");


  //WiFI managent part
  WiFi.mode(WIFI_STA);
  //WiFi.begin(ssid, password);
  WiFi_init();
  //wm.resetSettings(); //reset settings - wipe credentials for testing
  //stop until connection wouldn't be established
  while (WiFi.status() != WL_CONNECTED) 
  {
    WiFi_CheckConnection();
  }

  ////////////////////////////////
  // MDNS INIT
  if (MDNS.begin(host)) {
    MDNS.addService("http", "tcp", 80);
    Serial.print(F("Open http://"));
    Serial.print(host);
    Serial.println(F(".local to access WeatherStation interface"));
  }

  server.on("/", handleRoot);
  server.on("/json", handleJSON);
  server.on("/configmode", handleConfigMode);
  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("HTTP server started");


  //init BME280 sensor
  if (!bme.begin(SDA_pin, SCL_pin)) 
  {
    Serial.println("Could not find a valid BME280 sensor, check wiring!");
  } 

  //MLX
  mlx.begin(SDA_pin, SCL_pin);  

  //Dallas Sensors
  ds18b20.begin();
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
      //HTTP_SendData();
      HTTP_sendJSON();
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
    //OW_Temp1 = getOneWireTemp(OW_Temp1Addr);
    ds18b20.requestTemperatures(); 
    OW_Temp1 = ds18b20.getTempCByIndex(0);
    Serial.print("[!OW1:");
    Serial.print(OW_Temp1);
    Serial.println("]");

    _lastReadTime_OW= currenttime;
  }

  if (_lastReadTime_MLX ==0 || (currenttime - _lastReadTime_MLX) > MLX_READ_INTERVAL)
  {
    bOutput=true;
    ReadMLXvalues(mlxAmb, mlxObj);

    _lastReadTime_MLX = currenttime;
  }

  if ((currenttime - _lastWiFi_checkattempt) > WIFI_CHECK_CONNECTION_INTERVAL)
  {
    WiFi_CheckConnection();
    _lastWiFi_checkattempt = currenttime;
  }

  /*if (bOutput)
  {
    Serial.println(F("[!END]\r\n"));
  }
  */
  //delay(5000);
}
