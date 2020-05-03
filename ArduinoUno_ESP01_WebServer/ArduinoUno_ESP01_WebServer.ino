/*
  WATERING CONTROL
  by Boris Emchenko
  
 Changes:
   ver 0.8i2 [26 644/29 834] - also get current params
*/
#include "WiFiEsp.h"

//Compile version
#define VERSION "0.1"
#define VERSION_DATE "20200503"


// Emulate Serial1 on pins 6/7 if not present
#ifndef HAVE_HWSERIAL1
#include "SoftwareSerial.h"
SoftwareSerial Serial1(6, 5); // RX, TX
#endif

char ssid[] = "BATMAJ";            // your network SSID (name)
char pass[] = "8oknehcmE";        // your network password
int status = WL_IDLE_STATUS;     // the Wifi radio's status
int reqCount = 0;                // number of requests received

WiFiEspServer server(80);

String readBuffer;

#define DHT_PIN 4
// DHT functions enumerated
enum {DHT22_SAMPLE, DHT_TEMPERATURE, DHT_HUMIDITY, DHT_DATAPTR};
// DHT error codes enumerated
enum {DHT_OK = 0, DHT_ERROR_TIMEOUT = -1, DHT_ERROR_CRC = -2, DHT_ERROR_UNKNOWN = -3};
float dhtTemp = -100;
float dhtHum =0;

#define RELAY_PUMP_PIN 7


unsigned long currenttime;
unsigned long _lastReadTime_DHT=0;
#define DHT_READ_INTERVAL 3000

void setup()
{
  Serial.begin(9600);
  Serial.print("Watering Station v");
  Serial.print(VERSION);
  Serial.print(" [");
  Serial.print(VERSION_DATE);
  Serial.println("]");
  
  
  Serial1.begin(9600);

  // initialize ESP module
  WiFi.init(&Serial1);

  // check for the presence of the shield
  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("WiFi shield not present");
    // don't continue
    while (true);
  }

  // attempt to connect to WiFi network
  while ( status != WL_CONNECTED) {
    Serial.print("Attempting to connect to WPA SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network
    status = WiFi.begin(ssid, pass);
  }

  Serial.println("You're connected to the network");
  printWifiStatus();
  
  // start the web server on port 80
  server.begin();
  
  pinMode(RELAY_PUMP_PIN, OUTPUT);
  digitalWrite(RELAY_PUMP_PIN, HIGH);
}



void loop()
{
  // listen for incoming clients
  WiFiEspClient client = server.available();
  
  // IF ANY CONNECTION, HANDLE IT
  if (client) {
    Serial.println("New client connected");
    // an http request ends with a blank line
    boolean currentLineIsBlank = true;
    readBuffer="";
    while (client.connected()) 
    {
      if (client.available()) {
        char c = client.read();
        readBuffer += c;
        Serial.write(c);
    
        // if you've gotten to the end of the line (received a newline
        // character) and the line is blank, the http request has ended,
        // so you can send a reply
        if (c == '\n' && currentLineIsBlank) {
          //Answer to client
          sendHttpResponse(client);
          break;
        }
        
        if (c == '\n') {
          currentLineIsBlank = true;  // you're starting a new line
        } else if (c != '\r') {
          currentLineIsBlank = false; // you've gotten a character on the current line
        }
      }
    }
    // give the web browser time to receive the data
    delay(10);
    // close the connection:
    client.stop();
    Serial.println("Client disconnected");
    
    //Parse commands
    if (readBuffer.indexOf("GET /dht")>=0)
    {
    }
    else if (readBuffer.indexOf("GET /pumpon")>=0)
    {
      switchOn();
    }
    else if (readBuffer.indexOf("GET /pumpoff")>=0)
    {
      switchOff();
    }
  }
  else
  {
    // IF NO CONNECTIONS, READ SENSOR DATA, MAKE CALCULATIONS, ETC
    currenttime = millis();

    //Analog Read
    //Serial.print("Analog input A0: ");
    //Serial.println(analogRead(0));

    //DHT Read
    if ((currenttime - _lastReadTime_DHT) > DHT_READ_INTERVAL)
    {
      readDHTSensor(dhtTemp,dhtHum);
      _lastReadTime_DHT= currenttime;
    }
  }
 

  // END OF CYCLE
  delay(10);
}
