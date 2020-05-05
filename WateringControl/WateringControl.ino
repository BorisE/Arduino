/*
  WATERING CONTROL
  (c) 2020 by Boris Emchenko
  
 Changes:
   ver 0.5 2020/05/05 [20146/1333] - set params from web
   ver 0.4 2020/05/05 [19386] - optimizing command parsing (to use less readbuffer, because we got into global memory restriction)
                              - restrict pump running: by time and by very wet
                              - amp correction pedestal
                              - string messages optimized
   ver 0.3 2020/05/04 [19412] - current sensor added, some web pages logic reworked
   ver 0.2 2020/05/03 [18872] - web pages with redirect (rnd fix)
   ver 0.1 2020/05/03 [18102] - Starting release (WiFi, DHT wo lib, pump relay)
*/
#include "WiFiEsp.h"
#include <avr/pgmspace.h>

//Compile version
#define VERSION "0.5"
#define VERSION_DATE "20200505"

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
#define SOIL_VERYWET_THRESHOLD 100

#define DHT_PIN 4
enum {DHT22_SAMPLE, DHT_TEMPERATURE, DHT_HUMIDITY, DHT_DATAPTR};  // DHT functions enumerated
enum {DHT_OK = 0, DHT_ERROR_TIMEOUT = -1, DHT_ERROR_CRC = -2, DHT_ERROR_UNKNOWN = -3};  // DHT error codes enumerated
float dhtTemp = -100;
float dhtHum =0;
unsigned long _lastReadTime_DHT=0;
#define DHT_READ_INTERVAL 3000

#define RELAY_PUMP_PIN 7
byte PumpStatus;
unsigned long pumpstarttime;
unsigned long MAX_PUMP_RUNTIME = 60000;

#define AMP_PIN A5
#define AMP_SAMPLING_NUMBER 150
float AcsValueF=0.0;
unsigned long _lastReadTime_AMP=0;
float AMP_Correction = 0.308;
#define AMP_READ_INTERVAL 1111

unsigned long currenttime;

unsigned int cmd=0;
#define CMD_MAIN       0
#define CMD_PUMP_ON   10
#define CMD_PUMP_OFF  11
#define CMD_SETPARAM   2

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
    Serial.println("[New client]");
    // an http request ends with a blank line
    boolean currentLineIsBlank = true;
    readBuffer="";
    cmd=CMD_MAIN;
    while (client.connected()) 
    {
      if (client.available()) 
      {
        char c = client.read();
        readBuffer += c;
        //Serial.write(c);
    
        // if you've gotten to the end of the line (received a newline
        // character) and the line is blank, the http request has ended,
        // so you can send a reply
        if (c == '\n') 
        {
          if (readBuffer.indexOf("GET /")>=0)
          {
            Serial.println(readBuffer);
            if (readBuffer.indexOf("GET /pumpon")>=0)
            {
              cmd=CMD_PUMP_ON;
            }
            else if (readBuffer.indexOf("GET /pumpoff")>=0)
            {
              cmd=CMD_PUMP_OFF;
            }
            else if (readBuffer.indexOf("GET /set/")>=0)
            {
              cmd=CMD_SETPARAM;
              unsigned long tempParam= 0;
              
              if (tempParam = getParamLong("pumprun"))
              {
                MAX_PUMP_RUNTIME = tempParam;
                Serial.print("PR set: ");
                Serial.println(MAX_PUMP_RUNTIME);
              }
              else
              {
                Serial.print("Not Found");
              }
            }
          }
          readBuffer =""; // Очистить буфер, чтобы не занимал память
                          // Т.е. мы храним в буффере только последнюю строчку

          if (currentLineIsBlank) 
          {
            Serial.println(readBuffer);
            
            //Parse commands
            switch (cmd)
            {
              case CMD_PUMP_ON:
                //Serial.println("GET /pumpon");
                switchOn();
                sendHttpResponse_goRoot(client);
                break;
              case CMD_PUMP_OFF:
                //Serial.println("GET /pumpoff");
                switchOff();
                sendHttpResponse_goRoot(client);
                break;
              case CMD_SETPARAM:
                sendHttpResponse_goRoot(client);
                break;
              default:
              //Answer to client
                sendHttpResponse_MainPage(client);
            } 
  
            break;
          }
        }
        
        if (c == '\n') {
          currentLineIsBlank = true;  // you're starting a new line
        } else if (c != '\r') {
          currentLineIsBlank = false; // you've gotten a character on the current line
        }
      }
    }
    // give the web browser time to receive the data
    delay(30);
    // close the connection:
    client.stop();
    Serial.println("Sending done.");
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
    if (PumpStatus == LOW)
    {
      if ((currenttime - pumpstarttime) > MAX_PUMP_RUNTIME)
      {
        Serial.println("[OFF timer]");
        switchOff();
      }
      else if (PumpStatus == LOW && (Soil_1_Val < SOIL_VERYWET_THRESHOLD))
      {
        Serial.println("[OFF VERYWET]");
        switchOff();
      }
    }
      
 

    //Pump current 
    if ((currenttime - _lastReadTime_AMP) > AMP_READ_INTERVAL)
    {
      GetAMPValue();
      _lastReadTime_AMP= currenttime;

      Serial.print("[!Soil:");
      Serial.print(Soil_1_Val);
      Serial.println("]");
    }
    
  }
 

  // END OF CYCLE
  delay(10);
}
