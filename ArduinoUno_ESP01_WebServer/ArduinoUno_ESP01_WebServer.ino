/*
 WiFiEsp example: WebServer

 A simple web server that shows the value of the analog input 
 pins via a web page using an ESP8266 module.
 This sketch will print the IP address of your ESP8266 module (once connected)
 to the Serial monitor. From there, you can open that address in a web browser
 to display the web page.
 The web page will be automatically refreshed each 20 seconds.

 For more details see: http://yaab-arduino.blogspot.com/p/wifiesp.html
*/

#include "WiFiEsp.h"
#include "DHT.h"


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

#define DHTPIN 4     // what pin we're connected to
#define DHT_PIN 4
#define DHTTYPE DHT22   // DHT 22  (AM2302)
DHT dht(DHTPIN, DHTTYPE);
float dhtTemp = -100;
float dhtHum =0;

#define RELAY_PUMP_PIN 7

void setup()
{
  // initialize serial for debugging
  Serial.begin(9600);
  // initialize serial for ESP module
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
  
  //Start DHT lib
  dht.begin();

  pinMode(RELAY_PUMP_PIN, OUTPUT);
  digitalWrite(RELAY_PUMP_PIN, HIGH);
}



void loop()
{
  // listen for incoming clients
  WiFiEspClient client = server.available();
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
    // Read data
    //Serial.print("Analog input A0: ");
    //Serial.println(analogRead(0));
    readDHTSensor(dhtTemp,dhtHum);
  }
  

  delay(10);
  
}


void printWifiStatus()
{
  // print the SSID of the network you're attached to
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your WiFi shield's IP address
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);
  Serial.println();
}

void sendHttpResponse(WiFiEspClient client)
{
    Serial.println("Sending response");
    // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
    // and a content-type so the client knows what's coming, then a blank line:
    // send a standard http response header
    // use \r\n instead of many println statements to speedup data send
    client.print(
      "HTTP/1.1 200 OK\r\n"
      "Content-Type: text/html\r\n"
      "Connection: close\r\n"  // the connection will be closed after completion of the response
      "Refresh: 20\r\n"        // refresh the page automatically every 20 sec
      "\r\n");
    client.print("<!DOCTYPE HTML>\r\n");
    client.print("<html>\r\n");
    client.print("<h1>Hello World!</h1>\r\n");
    client.print("Requests received: ");
    client.print(++reqCount);
    client.print("<br>\r\n");
    client.print("Analog input A0: ");
    client.print(analogRead(0));
    client.print("<br>\r\n");

    client.print("DHT temp: ");
    client.print(dhtTemp);
    client.print("<br>\r\n");
    client.print("DHT humidity: ");
    client.print(dhtHum);
    client.print("<br>\r\n");

    client.print("Pump switch: ");
    client.print(digitalRead(RELAY_PUMP_PIN));
    client.print("<br>\r\n");
  
    client.print("</html>\r\n");

    // The HTTP response ends with another blank line:
    client.println();
}

void swapInt( int*a, int *b)
{
     int c = *a;
     *a = *b;
     *b = c;
}

void readDHTSensor(float &t, float &h)
{
  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  h = dht.readHumidity();
  t = dht.readTemperature();

  // check if returns are valid, if they are NaN (not a number) then something went wrong!
  if (isnan(t) || isnan(h)) {
    Serial.println("Failed to read from DHT");
  } else {
    /*
    Serial.print("Humidity: "); 
    Serial.print(h);
    Serial.print(" %\t");
    Serial.print("Temperature: "); 
    Serial.print(t);
    Serial.println(" *C");
    */
  }
}


void switchOn()
{
  digitalWrite(RELAY_PUMP_PIN, LOW);
  Serial.println("Switching PUMP ON"); 

}
void switchOff()
{
  digitalWrite(RELAY_PUMP_PIN, HIGH);
  Serial.println("Switching PUMP OFF"); 
}
