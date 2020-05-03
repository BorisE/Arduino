/*
  WATERING CONTROL
  (c) 2020 by Boris Emchenko
  
 Changes:
   ver 0.2 2020/05/03 [18872] - web pages with redirect (rnd fix)
   ver 0.1 2020/05/03 [18102] - Starting release (WiFi, DHT wo lib, pump relay)
*/
#include "WiFiEsp.h"

//Compile version
#define VERSION "0.2"
#define VERSION_DATE "20200503"

// Emulate Serial1 on pins 6/7 if not present
#ifndef HAVE_HWSERIAL1
#include "SoftwareSerial.h"
SoftwareSerial Serial1(6, 5); // RX, TX
#endif

char ssid[] = "BATMAJ";          // your network SSID (name)
char pass[] = "8oknehcmE";       // your network password
int status = WL_IDLE_STATUS;     // the Wifi radio's status
int reqCount = 0;                // number of requests received
String readBuffer;
WiFiEspServer server(80);

#define SOIL_1_PIN A0
int Soil_1_Val=-1;

#define DHT_PIN 4
enum {DHT22_SAMPLE, DHT_TEMPERATURE, DHT_HUMIDITY, DHT_DATAPTR};  // DHT functions enumerated
enum {DHT_OK = 0, DHT_ERROR_TIMEOUT = -1, DHT_ERROR_CRC = -2, DHT_ERROR_UNKNOWN = -3};  // DHT error codes enumerated
float dhtTemp = -100;
float dhtHum =0;

#define RELAY_PUMP_PIN 7
byte PumpStatus;

unsigned long currenttime;
unsigned long _lastReadTime_DHT=0;
#define DHT_READ_INTERVAL 3000

void setup()
{
  Serial.begin(9600);
  Serial.print("Watering Control v");
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

  randomSeed(analogRead(A5));
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
        //Serial.write(c);
    
        // if you've gotten to the end of the line (received a newline
        // character) and the line is blank, the http request has ended,
        // so you can send a reply
        if (c == '\n' && currentLineIsBlank) 
        {
          Serial.println(readBuffer);
          
          //Parse commands
          if (readBuffer.indexOf("GET /dht")>=0)
          {
            Serial.println("GET /dht");
            sendHttpResponse_goRoot(client);
          }
          else if (readBuffer.indexOf("GET /pumpon")>=0)
          {
            Serial.println("GET /pumpon");
            switchOn();
            sendHttpResponse_goRoot(client);
          }
          else if (readBuffer.indexOf("GET /pumpoff")>=0)
          {
            Serial.println("GET /pumpoff");
            switchOff();
            sendHttpResponse_goRoot(client);
          }
          else
          {
            //Answer to client
            sendHttpResponse_MainPage(client);
          } 

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
    Serial.println("Sending done. Client disconnected");
  }
  else
  {
    // IF NO CONNECTIONS, READ SENSOR DATA, MAKE CALCULATIONS, ETC
    currenttime = millis();

    //Analog Read
    Soil_1_Val = analogRead(SOIL_1_PIN);

    //DHT Read
    if ((currenttime - _lastReadTime_DHT) > DHT_READ_INTERVAL)
    {
      readDHTSensor(dhtTemp,dhtHum);
      _lastReadTime_DHT= currenttime;
    }

    //Pump Status
    PumpStatus = digitalRead(RELAY_PUMP_PIN);
  }
 

  // END OF CYCLE
  delay(10);
}
