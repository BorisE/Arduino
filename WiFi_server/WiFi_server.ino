#include <SoftwareSerial.h>
#include "WiFly.h"

#define SSID      "BATMAJ"
#define KEY       "8oknehcmE"
// check your access point's security mode, mine was WPA20-PSK
// if yours is different you'll need to change the AUTH constant, see the file WiFly.h for avalable security codes
#define AUTH      WIFLY_AUTH_WPA2_PSK

int flag = 0;

// Pins' connection
// Arduino       WiFly
//  2    <---->    TX
//  3    <---->    RX

SoftwareSerial wiflyUart(2, 3); // create a WiFi shield serial object
WiFly wifly(&wiflyUart); // pass the wifi siheld serial object to the WiFly class

void setup()
{
      pinMode(LED_BUILTIN, OUTPUT);
    
    wiflyUart.begin(9600); // start wifi shield uart port
    Serial.begin(9600); // start the arduino serial port
    Serial.println("--------- WIFLY Webserver --------");

    // wait for initilization of wifly
    delay(1000);

    wifly.reset(); // reset the shield
    delay(1000);
    //set WiFly params

    wifly.sendCommand("set ip local 80\r"); // set the local comm port to 80
    delay(100);

    wifly.sendCommand("set comm remote 0\r"); // do not send a default string when a connection opens
    delay(100);

    wifly.sendCommand("set comm open *OPEN*\r"); // set the string that the wifi shield will output when a connection is opened
    delay(100);

    Serial.println("Join " SSID );
    if (wifly.join(SSID, KEY, AUTH)) {
        Serial.println("OK");
    } else {
        Serial.println("Failed");
    }

    delay(5000);

    wifly.sendCommand("get ip\r");
    char c;

    while (wifly.receive((uint8_t *)&c, 1, 300) > 0) { // print the response from the get ip command
        Serial.print((char)c);
    }

    Serial.println("Web server ready");

}

void loop()
{

// *OPEN*GET / HTTP/1.1
// Host: 192.168.0.231:2000
// User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64; rv:75.0) Gecko/20100101 Firefox/75.0
// Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,*/*;q=0.8
// Accept-Language: en-GB,en;q=0.5
// Accept-Encoding: gzip, deflate
// Connection: keep-alive
// Upgrade-Insecure-Requests: 1



 Serial.println("ON");
  digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(2000);                       // wait for a second
  
  Serial.println("OFF");
  digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW
  delay(1000);                       // wait for a second

  
    if(wifly.available())
    { // the wifi shield has data available
        if(wiflyUart.find("*OPEN*")) // see if the data available is from an open connection by looking for the *OPEN* string
        {
            Serial.println("New Browser Request!");
            delay(1000); // delay enough time for the browser to complete sending its HTTP request string
            
            if(wiflyUart.find("GET /dht")) // see if the data available is from an open connection by looking for the *OPEN* string
            {
              Serial.println("DHT requested");
              char c1=wiflyUart.read();
              char c2=wiflyUart.read();
              Serial.print("PARAM:");
              Serial.print(c1);
              Serial.print(c2);
              Serial.println();
              
            }
            
            // send HTTP header
            wiflyUart.println("HTTP/1.1 200 OK");
            wiflyUart.println("Content-Type: text/html; charset=UTF-8");
            wiflyUart.println("Content-Length: 244"); // length of HTML code
            wiflyUart.println("Connection: close");
            wiflyUart.println();

            // send webpage's HTML code
            wiflyUart.print("<html>");
            wiflyUart.print("<head>");
            wiflyUart.print("<title>My WiFI Shield Webpage</title>");
            wiflyUart.print("</head>");
            wiflyUart.print("<body>");
            wiflyUart.print("<h1>Hello World!</h1>");
            wiflyUart.print("<h3>This website is served from my WiFi module</h3>");
            wiflyUart.print("<a href=\"http://yahoo.com\">Yahoo!</a> <a href=\"http://google.com\">Google</a>");
            wiflyUart.print("<br/><button>My Button</button>");
            wiflyUart.print("</body>");
            wiflyUart.print("</html>");
        }
  }
 
}
