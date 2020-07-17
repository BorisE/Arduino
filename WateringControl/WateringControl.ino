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

  JSON Response: {"R":1,"S":358,"T":-100.00,"H":0.00,"P":0,"C":0.47}
  R  1    - requests count
  S 358   - Soil sensor 1 value
  T -100  - Temperature from DHT sensor
  H 0     - Humidity from DHT sensor
  P 0     - Pump status (on|off)
  C 0.47  - Pump current

 
 Changes:
   ver 1.0rc 2020/07/15 [22960/1083] - send data more ofter when pump is on
   ver 0.9 2020/06/20 [22942/1083] - pushbutton to start/stop pump
   ver 0.8 2020/05/09 [22582/1074] - send data by http get
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
#define VERSION "1.0"
#define VERSION_DATE "20200715"

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

#define RELAY_PUMP_PIN 7
byte PumpStatus;                        // PumpStatus: 1 - on, 0 - off (digitalPin in fact is inversed: 0 (LOW) - on, 1 (HIGH) - off)
unsigned long pumpstarttime;
unsigned long MAX_PUMP_RUNTIME = 60000; // Maximum Pump Runtime in millis

#define AMP_PIN A5
#define AMP_SAMPLING_NUMBER 150         // number of consecutive sensor reads to average noise
float AcsValueF=0.0;
unsigned long _lastReadTime_AMP=0;
float AMP_Correction = 0;           // base correction pedestal to set zero

#define buttonPin 3                     // the number of the pushbutton pin (UNO: only 2 or 3)
#define DEBOUNCE_DELAY 300              // delay for button debounce
volatile byte buttonState = LOW;

unsigned int cmd=0;                     // curren command
#define CMD_MAIN       0
#define CMD_PUMP_ON   10
#define CMD_PUMP_OFF  11
#define CMD_SETPARAM   2

char ServerName[] = "192.168.0.199";    // for Sending Request
IPAddress ServerIP(192, 168, 0, 199);   // Sending Server ip address
unsigned long _lastTime_HTTPSENT;

unsigned long currenttime;              // millis from script start 
#define SEND_DATA_INTERVAL_NORMAL 60000
#define SEND_DATA_INTERVAL_PUMP 10000
#define DATA_READ_INTERVAL 11111
#define DHT_READ_INTERVAL 30000

bool bOutput=false;

/********************************************************
*     SETUP
 *******************************************************/
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

  // Push button
  pinMode(buttonPin, INPUT);
  attachInterrupt(digitalPinToInterrupt(buttonPin), ReadButtonPress, RISING);  
}


void loop()
{
  bOutput=false;

  // check push button
  if (buttonState == HIGH) {
    buttonState = LOW;    
    Serial.print(F("Button pushed, "));
    if (PumpStatus){
      Serial.println("off");
      switchOff();
    } else {
      Serial.println("on");
      switchOn();
    }
  }

  // listen for incoming clients
  WiFiEspClient WebClient = server.available();
  
  // IF ANY CONNECTION, HANDLE IT
  if (WebClient && (WebClient.remoteIP() != ServerIP)) {
    Serial.print(F("New request ["));
    Serial.print(WebClient.remoteIP());
    Serial.println("]");

    // an http request ends with a blank line
    boolean currentLineIsBlank = true;
    readBuffer="";
    cmd=CMD_MAIN;
    while (WebClient.connected()) 
    {
      if (WebClient.available()) 
      {
        char c = WebClient.read();
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
                sendHttpResponse_JSON(WebClient);
                break;
              case CMD_PUMP_OFF:
                switchOff();
                sendHttpResponse_JSON(WebClient);
                break;
              case CMD_SETPARAM:
                sendHttpResponse_Settings_JSON(WebClient);
                break;
              default:
                //Answer to client
                sendHttpResponse_JSON(WebClient);
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
    WebClient.stop();
    Serial.println(F("Sending done."));
  }
  else
  {
    // IF NO SERVER CONNECTIONS, READ SENSOR DATA, MAKE CALCULATIONS, ETC

    // Если это был не запрос на сервер, то вывести содержимое на экран и остановиться
    while (WebClient.available()) {
      char c = WebClient.read();
      Serial.write(c);
    }
    WebClient.stop();

    currenttime = millis();

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
      
    //if PUMP is on, then every cycle
    //Otherwise every given interval
    if (PumpStatus || ((currenttime - _lastReadTime_AMP) > DATA_READ_INTERVAL))
    {
      bOutput=true;
      //Pump status
      Serial.print("[!Pump:");
      Serial.print(PumpStatus);
      Serial.println("]");
      
      //Pump current
      GetAMPValue();

      //Soil sensor
      Soil_1_Val = analogRead(SOIL_1_PIN);
      Serial.print("[!Soil:");
      Serial.print(Soil_1_Val);
      Serial.println("]");

      _lastReadTime_AMP= currenttime;
    }

    //DHT Read vry time consuming
    //So read only in given interval
    if ((currenttime - _lastReadTime_DHT) > DHT_READ_INTERVAL)
    {
      bOutput=true;
      readDHTSensor(dhtTemp, dhtHum);
      _lastReadTime_DHT= currenttime;
    }

    //Send data in given interval of time
    //but if pump is on do it every cycle
    if ( (!PumpStatus && (currenttime - _lastTime_HTTPSENT) > SEND_DATA_INTERVAL_NORMAL) || (PumpStatus && (currenttime - _lastTime_HTTPSENT) > SEND_DATA_INTERVAL_PUMP))
    {
      httpRequest_Send(WebClient);
      delay(100);
      _lastTime_HTTPSENT= currenttime;
    }
    
    if (bOutput)
    {
      Serial.println(F("[!END]\r\n"));
    }
  }
 
  // END OF CYCLE
  delay(50);
}



void ReadButtonPress()
{
  static unsigned long lastread;
  if ((millis() - lastread) > DEBOUNCE_DELAY)
  {
    buttonState = digitalRead (buttonPin);
  }
  lastread = millis(); 
}
