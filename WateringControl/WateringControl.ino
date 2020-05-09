/*
  WATERING CONTROL
  (c) 2020 by Boris Emchenko

  usage through web:
  http://192.168.0.235/ - get JSON status
  http://192.168.0.235/pumpon - pump on
  http://192.168.0.235/pumpoff - pump off
  http://192.168.0.235/set/ - get parameters JSON
  http://192.168.0.235/set/VWT/400 - set parameter Very Wet Threshold to 400

  add ramdom parameter to bypass cache:
  http://192.168.0.235/pumpon/1023014
  
 Changes:
   ver 0.8 2020/05/09 [22540/1083] - send data by http get
   ver 0.7 2020/05/06 [20158/971] - moved some strings to flash memory
   ver 0.6 2020/05/06 [19958/1101] - moved to JSON responses
                                   - set param VWT
                                   - some optimization
   ver 0.5 2020/05/05 [20146/1333] - set params from web
   ver 0.4 2020/05/05 [19386] - optimizing command parsing (to use less readbuffer, because we got into global memory restriction)
                              - restrict pump running: by time and by very wet
                              - amp correction pedestal
                              - string messages optimized
   ver 0.3 2020/05/04 [19412] - current sensor added, some web pages logic reworked
   ver 0.2 2020/05/03 [18872] - web pages with redirect (rnd fix)
   ver 0.1 2020/05/03 [18102] - Starting release (WiFi, DHT wo lib, pump relay)
*/
//Compile version
#define VERSION "0.8"
#define VERSION_DATE "20200509"

#include "WiFiEsp.h"

// Emulate Serial1 on pins 6/5 if not present
#ifndef HAVE_HWSERIAL1
#include "SoftwareSerial.h"
SoftwareSerial Serial1(6, 5); // RX, TX
#endif
char ssid[] = "BATMAJ";          // your network SSID (name)
char pass[] = "8oknehcmE";       // your network password
int status = WL_IDLE_STATUS;     // the Wifi radio's status
unsigned int reqCount = 0;       // number of requests received
String readBuffer;               // current line from HTTP REQUEST
WiFiEspServer server(80);

#define SOIL_1_PIN A0
int Soil_1_Val=-1;
int SOIL_VERYWET_THRESHOLD=100;

#define DHT_PIN 4
enum {DHT22_SAMPLE, DHT_TEMPERATURE, DHT_HUMIDITY, DHT_DATAPTR};  // DHT functions enumerated
enum {DHT_OK = 0, DHT_ERROR_TIMEOUT = -1, DHT_ERROR_CRC = -2, DHT_ERROR_UNKNOWN = -3};  // DHT error codes enumerated
float dhtTemp = -100;
float dhtHum =0;
unsigned long _lastReadTime_DHT=0;
#define DHT_READ_INTERVAL 10000

#define RELAY_PUMP_PIN 7
byte PumpStatus;                        // PumpStatus: 1 - on, 0 - off (digitalPin in fact is inversed: 0 (LOW) - on, 1 (HIGH) - off)
unsigned long pumpstarttime;
unsigned long MAX_PUMP_RUNTIME = 60000; // Maximum Pump Runtime in millis

#define AMP_PIN A5
#define AMP_SAMPLING_NUMBER 150         // number of consecutive sensor reads to average noise
float AcsValueF=0.0;
unsigned long _lastReadTime_AMP=0;
float AMP_Correction = 0.308;           // base correction pedestal to set zero
#define AMP_READ_INTERVAL 1111

unsigned int cmd=0;                     // curren command
#define CMD_MAIN       0
#define CMD_PUMP_ON   10
#define CMD_PUMP_OFF  11
#define CMD_SETPARAM   2

char ServerName[] = "192.168.0.199";    // for Sending Request
IPAddress ServerIP(192, 168, 0, 199);   // Sending Server ip address
unsigned long SEND_DATA_INTERVAL = 60000;
unsigned long _lastTime_HTTPSENT;

unsigned long currenttime;              // millis from script start 

void setup()
{
  Serial.begin(9600);
  Serial.print(F("Watering Control v"));
  Serial.print(VERSION);
  Serial.print(" [");
  Serial.print(VERSION_DATE);
  Serial.println("]");
  
  
  Serial1.begin(9600);

  // initialize ESP module
  WiFi.init(&Serial1);

  // check for the presence of the shield
  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println(F("WiFi shield not present"));
    // don't continue
    while (true);
  }

  // attempt to connect to WiFi network
  while ( status != WL_CONNECTED) {
    Serial.print(F("Connecting to: "));
    Serial.println(ssid);
    // Connect to WPA/WPA2 network
    status = WiFi.begin(ssid, pass);
  }

  //Serial.println("Connected.");
  printWifiStatus();
  
  // start the web server on port 80
  server.begin();
  
  // init relay port and set it to CLOSED
  pinMode(RELAY_PUMP_PIN, OUTPUT);
  digitalWrite(RELAY_PUMP_PIN, HIGH);

  // Randomize
  randomSeed(analogRead(A4));
}


void loop()
{
  // listen for incoming clients
  //WiFiEspClient client = server.available();
  WiFiEspClient SendClient = server.available();
  
  // IF ANY CONNECTION, HANDLE IT
  if (SendClient && (SendClient.remoteIP() != ServerIP)) {
    Serial.print("New request [");
    Serial.print(SendClient.remoteIP());
    Serial.println("]");

    // an http request ends with a blank line

    boolean currentLineIsBlank = true;
    readBuffer="";
    cmd=CMD_MAIN;
    while (SendClient.connected()) 
    {
      if (SendClient.available()) 
      {
        char c = SendClient.read();
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
            if (readBuffer.indexOf("/pumpon")>=0)
            {
              cmd=CMD_PUMP_ON;
            }
            else if (readBuffer.indexOf("/pumpoff")>=0)
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
                Serial.print("MPR set: ");
                Serial.println(MAX_PUMP_RUNTIME);
              }
              else if (tempParam = getParamLong("verywet"))
              {
                SOIL_VERYWET_THRESHOLD = tempParam;
                Serial.print("VWT set: ");
                Serial.println(SOIL_VERYWET_THRESHOLD);
              }
              else
              {
                Serial.print(F("Param Not Found"));
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
                switchOn();
                sendHttpResponse_JSON(SendClient);
                break;
              case CMD_PUMP_OFF:
                switchOff();
                sendHttpResponse_JSON(SendClient);
                break;
              case CMD_SETPARAM:
                sendHttpResponse_Settings_JSON(SendClient);
                break;
              default:
                //Answer to client
                sendHttpResponse_JSON(SendClient);
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
    SendClient.stop();
    Serial.println(F("Sending done."));
  }
  else
  {
    // Если это был не запрос на сервер, то вывести содержимое на экран и остановиться
    while (SendClient.available()) {
      char c = SendClient.read();
      Serial.write(c);
    }
    SendClient.stop();
    
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
    PumpStatus = !digitalRead(RELAY_PUMP_PIN);
    if (PumpStatus)
    {
      if ((currenttime - pumpstarttime) > MAX_PUMP_RUNTIME)
      {
        Serial.println(F("[OFF timer]"));
        switchOff();
      }
      else if (PumpStatus && (Soil_1_Val < SOIL_VERYWET_THRESHOLD))
      {
        Serial.println(F("[OFF VERYWET]"));
        switchOff();
      }
    }
      
 
    //Pump current 
    if ((currenttime - _lastReadTime_AMP) > AMP_READ_INTERVAL)
    {
      Serial.print("[!Pump:");
      Serial.print(PumpStatus);
      Serial.println("]");
      
      GetAMPValue();
      _lastReadTime_AMP= currenttime;

      Serial.print("[!Soil:");
      Serial.print(Soil_1_Val);
      Serial.println("]");
    }

    //Send data
    if ((currenttime - _lastTime_HTTPSENT) > SEND_DATA_INTERVAL)
    {
      httpRequest_Send(SendClient);
      _lastTime_HTTPSENT= currenttime;
      delay(50);
    }
    
  }
 

  // END OF CYCLE
  delay(50);
}
