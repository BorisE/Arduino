/*
  Repeating Web client
 
 This sketch connects to a a web server and makes a request
 using a Wiznet Ethernet shield. You can use the Arduino Ethernet shield, or
 the Adafruit Ethernet shield, either one will work, as long as it's got
 a Wiznet Ethernet module on board.
 
 This example uses DNS, by assigning the Ethernet client with a MAC address,
 IP address, and DNS address.
 
 Circuit:
 * Ethernet shield attached to pins 10, 11, 12, 13
 
 created 19 Apr 2012
 by Tom Igoe
 
 http://arduino.cc/en/Tutorial/WebClientRepeating
 This code is in the public domain.
 
 */

// this method makes a HTTP connection to the server:
//          httpRequest(objectTemp,ambientTemp,bmp085_pressure_mmHg,bmp085_temperature,DHT22_h,DHT22_t,BH1750_val,BH1750_sens,BH1750_res_st,BH1750_res,BH1750_wait_time);

void httpRequest(double objectTemp, double ambientTemp, double bmp085_pressure_mmHg, double bmp085_temperature, double DHT22_h, double DHT22_t, double DHT22_2_h, double DHT22_2_t, 
      double BH1750_val, byte BH1750_sens, String BH1750_res_st, byte BH1750_res, int BH1750_wait_time, 
      float insideThermometerT, float outsideThermometerT, uint16_t WET_SensorValue, uint8_t WET_digitalValue, uint16_t RG_to_send, uint8_t RelState)
 {
  // if there's a successful connection:
  Serial.print("Connecting ");
  Serial.print(WEBSERVER_IP);
  Serial.print(":");
  Serial.print(WEBSERVER_PORT);
  Serial.println("...");
  if (WebClient.connect(WEBSERVER_IP, WEBSERVER_PORT)) {
    Serial.println("Sending data...");
    // send the HTTP PUT request:
    WebClient.print("GET "); Serial.print("GET ");
    WebClient.print(WEBPAGE); Serial.print(WEBPAGE);
    
    WebClient.print("ot="); Serial.print("ot=");
    WebClient.print(objectTemp); Serial.print(objectTemp);
    WebClient.print("&at="); Serial.print("&at=");
    WebClient.print(ambientTemp); Serial.print(ambientTemp);
    
    WebClient.print("&bp="); Serial.print("&bp=");
    WebClient.print(bmp085_pressure_mmHg); Serial.print(bmp085_pressure_mmHg);
    WebClient.print("&bt="); Serial.print("&bt=");
    WebClient.print(bmp085_temperature); Serial.print(bmp085_temperature);

    WebClient.print("&dh="); Serial.print("&dh=");
    WebClient.print(DHT22_h); Serial.print(DHT22_h);
    WebClient.print("&dt="); Serial.print("&dt=");
    WebClient.print(DHT22_t); Serial.print(DHT22_t);

    WebClient.print("&dh2="); Serial.print("&dh2=");
    WebClient.print(DHT22_2_h); Serial.print(DHT22_2_h);
    WebClient.print("&dt2="); Serial.print("&dt2=");
    WebClient.print(DHT22_2_t); Serial.print(DHT22_2_t);

    WebClient.print("&bhv="); Serial.print("&bhv=");
    WebClient.print(BH1750_val); Serial.print(BH1750_val);
    WebClient.print("&bhs="); Serial.print("&bhs=");
    WebClient.print(BH1750_sens); Serial.print(BH1750_sens);
    WebClient.print("&bhrs="); Serial.print("&bhrs=");
    WebClient.print(BH1750_res_st); Serial.print(BH1750_res_st);
    WebClient.print("&bhr="); Serial.print("&bhr="); 
    WebClient.print(BH1750_res); Serial.print(BH1750_res);
    WebClient.print("&bhw="); Serial.print("&bhw=");
    WebClient.print(BH1750_wait_time); Serial.print(BH1750_wait_time);

    WebClient.print("&owt1="); Serial.print("&owt1=");
    WebClient.print(insideThermometerT,3); Serial.print(insideThermometerT);
    WebClient.print("&owt2="); Serial.print("&owt2=");
    WebClient.print(outsideThermometerT,3); Serial.print(outsideThermometerT);
   
    WebClient.print("&wsv="); Serial.print("&wsv=");
    WebClient.print(WET_SensorValue); Serial.print(WET_SensorValue);
    WebClient.print("&wsf="); Serial.print("&wsf=");
    WebClient.print(WET_digitalValue); Serial.print(WET_digitalValue);

    WebClient.print("&rgc="); Serial.print("&rgc=");
    WebClient.print(RG_to_send); Serial.print(RG_to_send);
    WebClient.print("&rl1="); Serial.print("&rl1=");
    WebClient.print(RelState); Serial.print(RelState);
   
    WebClient.println(" HTTP/1.1"); Serial.println(" HTTP/1.1");
    WebClient.print("Host: "); Serial.print("Host: ");
    WebClient.println(WEBSERVER); Serial.println(WEBSERVER);
    WebClient.println("User-Agent: arduino-ethernet"); Serial.println("User-Agent: arduino-ethernet");
    WebClient.println("Connection: close"); Serial.println("Connection: close");
    WebClient.println(); Serial.println();

    // note the time that the connection was made:
    lastConnectionTime = millis();
  } 
  else {
    // if you couldn't make a connection:
    Serial.println("connection failed");
    Serial.println("disconnecting.");
    WebClient.stop();
  }
}

