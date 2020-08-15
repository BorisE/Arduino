/*
  WEATHER STATION MkII
  (c) 2020 by Boris Emchenko

  TODO:
  - mobile layout with cards
  - NTP server
  - data logging using SPIFFS
  - Capacitive rain sensor
  - UV sensor?
  - CO2 sensor?
  - OTA web update
  - Deepsleep mode?

 Changes:
   ver 1.11 2020/08/15 [430436/31744]
                      - memory (F(..)) optimization
   ver 1.1 2020/08/09 [430424/32244]
                      - added BH1750FVI (lux) sensor
                      - prepared (but not moved yet!) new HTML templates
   ver 1.02 2020/08/07 [427520/32200]
                      - ds18b20 connection tests to pass out of scale values
                      - config file changed to config.cfg
   ver 1.01 2020/08/06 [427400/32176]
                      - added SDA,SCL,DHT22 pin to config
                      - needs custom WiFiManager v2.0.3-alpha_0.3
   ver 1.00 2020/08/06 [426688/32000]
                      - store settings in special structure
                      - convert config to JSON format
                      - load / store json config on SPIFFS
                      - restart ESP on config chage (to take hardware settings into account)
   ver 1.00a2 2020/08/06 [385944/31656]
                      - blinking led (using ticker.h) during config mode
                      - some optimization
   ver 1.00a1 2020/08/05 [385764/31596]
                      - config portal is working!
                      - custom pararmeters to wificonfig (POST_URL and OneWirePin)
                      - WiFiManager lib customized (on 2.0.3-alpha_0.1): back redirect, WiFiManagerParameter->init method made public
   ver 0.92 2020/08/03 [383448/31388] 
                      - more WiFi connectivity optimization
   ver 0.91 2020/08/03 [383416/31388] 
                      - Solve issue with entering ConfigPortal in the middle on running
                      - Optimizing WiFi connectivity checking
                      - Webpages correction
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
#define VERSION "1.11"
#define VERSION_DATE "20200815"

#include <FS.h>          // this needs to be first, or it all crashes and burns...
#include <WiFiManager.h> // https://github.com/tzapu/WiFiManager
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h> // https://github.com/bblanchon/ArduinoJson

#include <Wire.h>
#include <BME280_I2C.h>
#include <OneWire.h>
#include <MLX90614.h>
#include <DallasTemperature.h>
#include <BH1750.h>

#include <Ticker.h>//for LED status

//#ifndef STASSID
//#define STASSID "BATMAJ"
//#define STAPSK  ""
//#endif
//const char* ssid = STASSID;
//const char* password = STAPSK;
const char* ssid = "WeatherStation";
const char* host = "weather";

struct Config {
  char POST_URL[101];
  uint8_t OneWirePin;
  uint8_t I2CSDAPin;
  uint8_t I2CSCLPin;
  uint8_t DHT22Pin;
};
const char *configFilename = "/config.cfg";  
Config config;                              // <- global configuration object


#define WIFI_CONFIG_PORTAL_WAITTIME  30
#define WIFI_CONFIG_PORTAL_WAITTIME_STARTUP  60

ESP8266WebServer server(80);

#define DEFAULT_POST_URL "http://192.168.0.199/weather/adddata.php"
//char POST_URL[101] = "http://192.168.0.199/weather/adddata.php"; //Where to post data
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

#define NONVALID_TEMPERATURE -100
#define NONVALID_PRESSURE 0
#define NONVALID_HUMIDITY 0
#define NONVALID_LUX -100

Ticker ticker;
const int STATUS_LED = LED_BUILTIN;

#define DHT_PINold D11
#define DHT_PIN_DEFAULT D11
enum {DHT22_SAMPLE, DHT_TEMPERATURE, DHT_HUMIDITY, DHT_DATAPTR};  // DHT functions enumerated
enum {DHT_OK = 0, DHT_ERROR_TIMEOUT = -1, DHT_ERROR_CRC = -2, DHT_ERROR_UNKNOWN = -3};  // DHT error codes enumerated
float dhtTemp = NONVALID_TEMPERATURE;
float dhtHum =0;
unsigned long _lastReadTime_DHT=0;

//I2C wire
#define SDA_PIN_DEFAULT D3
#define SCL_PIN_DEFAULT D4

// Create BME280 object
#define MY_BME280_ADDRESS (0x76)
BME280_I2C bme(MY_BME280_ADDRESS);
#define SEALEVELPRESSURE_HPA (1013.25)
float bmePres = NONVALID_PRESSURE;
float bmeTemp = NONVALID_TEMPERATURE;
float bmeHum  = NONVALID_HUMIDITY;
unsigned long _lastReadTime_BME=0;


#define ONE_WIRE_BUS_PIN_DEFAULT D6 // Data wire is plugged into this port 
//uint8_t ONE_WIRE_BUS_PIN = ONE_WIRE_BUS_PIN_DEFAULT;
//char ONE_WIRE_BUS_PIN_ST[4];
OneWire  OneWireBus;  

//ROM = 28 6D A3 68 4 0 0 F8
uint8_t OW_Temp1Addr[8] = { 0x28, 0x6D, 0xA3, 0x68, 0x4, 0x0, 0x0, 0xF8 };
float OW_Temp1=NONVALID_TEMPERATURE;
unsigned long _lastReadTime_OW=0;

DallasTemperature ds18b20(&OneWireBus);

// MLX90614 part
MLX90614 mlx = MLX90614();
float mlxAmb = NONVALID_TEMPERATURE;
float mlxObj = NONVALID_TEMPERATURE;
unsigned long _lastReadTime_MLX=0;

//BH1750FVI light sensor part
#define BH1750_ADDR 0x23        // if ADD pin to GROUND, 0x23; if to VCC - 0x5C
BH1750 lightMeter (BH1750_ADDR);
float bh1750Lux = NONVALID_LUX;
unsigned long _lastReadTime_BH1750=0;

unsigned long currenttime;              // millis from script start 
#define POST_DATA_INTERVAL    120000
#define JS_UPDATEDATA_INTERVAL  10000
#define DHT_READ_INTERVAL     10000
#define BME_READ_INTERVAL     10000
#define OW_READ_INTERVAL      10000
#define MLX_READ_INTERVAL     10000
#define BH1750_READ_INTERVAL  10000

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

  // Greeting message
  Serial.println(F("WEATHER STATION MkII"));
  Serial.print ("v");
  Serial.print (VERSION);
  Serial.print (" [");
  Serial.print (VERSION_DATE);
  Serial.println ("]");

  //Load config data
  LoadDefaults();
  LoadConfigData();

  ////////////////////////////////
  // WiFI managent part
  WiFi.mode(WIFI_STA);
  //WiFi.begin(ssid, password);
  //WiFi_init();
  //wm.resetSettings(); //reset settings - wipe credentials for testing
  //stop until connection wouldn't be established
  //while (WiFi.status() != WL_CONNECTED) <- don't block after timeout. Measure and then try again (in loop cycle)
  {
    WiFi_CheckConnection(WIFI_CONFIG_PORTAL_WAITTIME_STARTUP);
  }

  ////////////////////////////////
  // MDNS INIT
  if (MDNS.begin(host)) {
    MDNS.addService("http", "tcp", 80);
    Serial.print(F("Open http://"));
    Serial.print(host);
    Serial.println(F(".local to access WeatherStation interface"));
  }

  ////////////////////////////////
  // WebServer INIT
  server.on("/", handleRoot);
  server.on("/json", handleJSON);
  server.on("/configmode", handleConfigMode);
  server.on("/ping", handlePingRequest);
  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println(F("HTTP server started"));

  ////////////////////////////////
  // START HARDWARE
  Wire.begin(config.I2CSDAPin, config.I2CSCLPin);

  //init BME280 sensor
  if (!bme.begin(config.I2CSDAPin, config.I2CSCLPin)) 
  {
    Serial.println(F("Could not find a valid BME280 sensor, check wiring!"));
  } 

  //MLX
  mlx.begin(config.I2CSDAPin, config.I2CSCLPin);  

  //Dallas Sensors
  OneWireBus.begin(config.OneWirePin);
  ds18b20.begin();
  Serial.print(F("OneWire devices: "));
  Serial.println(ds18b20.getDeviceCount());
  ds18b20.getAddress(OW_Temp1Addr, 0); //try to read address for device 0

  //BH1750
  lightMeter.begin(BH1750::ONE_TIME_HIGH_RES_MODE);
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
      /* try to send data. test ret status. 
       * if no connection start CheckConnection procedure  */
      if (!HTTP_sendJSON())
      {
        server.stop(); // stop web server because of conflict with WiFi manager
        WiFi_CheckConnection(WIFI_CONFIG_PORTAL_WAITTIME);
        server.begin(); // start again
      }
      _last_HTTP_SEND = currenttime;
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
    if (ds18b20.isConnected(OW_Temp1Addr))
    {
      OW_Temp1 = ds18b20.getTempC(OW_Temp1Addr);
    }else{
      OW_Temp1 = NONVALID_TEMPERATURE;
    }
    
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

  if (_lastReadTime_BH1750 ==0 || (currenttime - _lastReadTime_BH1750) > BH1750_READ_INTERVAL)
  {
    bOutput=true;
    bh1750Lux  = measureLight();

    _lastReadTime_BH1750 = currenttime;
  }



  /*if (bOutput)
  {
    Serial.println(F("[!END]\r\n"));
  }
  */
  //delay(5000);
}
