#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

#ifndef STASSID
#define STASSID "BATMAJ"
#define STAPSK  "8oknehcmE"
#endif

const char* ssid = STASSID;
const char* password = STAPSK;

ESP8266WebServer server(80);

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
static const uint8_t D7   = 13;
static const uint8_t D8   = 0;
static const uint8_t D9   = 2;
static const uint8_t D10  = 15;
static const uint8_t D11  = 13;
static const uint8_t D12  = 12;
static const uint8_t D13  = 14;
static const uint8_t D14  = 4;
static const uint8_t D15  = 5;
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


unsigned long currenttime;              // millis from script start 
#define DHT_READ_INTERVAL 30000

bool bOutput=false;

/********************************************************
*     SETUP
 *******************************************************/
void setup(void) {
  pinMode(STATUS_LED, OUTPUT);
  digitalWrite(STATUS_LED, LOW);
  Serial.begin(115200);
  
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("");

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
}

void loop(void) {
  bOutput=false;
  currenttime = millis();
  
  server.handleClient();
  MDNS.update();

   //DHT Read vry time consuming
  //So read only in given interval
  if ((currenttime - _lastReadTime_DHT) > DHT_READ_INTERVAL)
  {
    bOutput=true;
    readDHTSensor(dhtTemp, dhtHum);
    _lastReadTime_DHT= currenttime;
  }

  if (bOutput)
  {
    Serial.println(F("[!END]\r\n"));
  }
  
}
