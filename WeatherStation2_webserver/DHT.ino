// DHT11 and DHT22 sensor reading demo
// by 'jurs' for Arduino Forum
// https://forum.arduino.cc/index.php?topic=308644.msg2142725#msg2142725

int dhtCall(byte function)
// input parameters are the data pin and one of the DHT functions
// return value is DHT error code with function DHT11_SAMPLE or DHT22_SAMPLE
// alsways do sampling with DHT_OK result before calling other functions
// return value is temperature with function DHT_TEMPERATURE
// return value is humidity with function DHT_HUMIDITY
// return value is pointer to byte array containing raw data with function DHT_DATAPTR
{
  static int temperature = -999;
  static int humidity = -999;
  static byte data[5]; // 5 bytes to receive 40 data bits
  unsigned int loopCnt; // loop counter
  byte sum;  // checksum
#define DHT_LOOPS 1800 //saw 55 ms, that for ESP2866 equals 4400

  switch (function)
  {
    case DHT22_SAMPLE: // REQUEST DHT22 SAMPLE
      
      yield();

      // First set data line low for a period according to sensor type
      pinMode(configData.DHT22Pin, OUTPUT);
      digitalWrite(configData.DHT22Pin, LOW);
      //delayMicroseconds(1000);  // data sheet says "at least 1ms"
      delay(1);
      
      pinMode(configData.DHT22Pin, INPUT_PULLUP);
      
      loopCnt = DHT_LOOPS;
      while (digitalRead(configData.DHT22Pin) == HIGH) if (loopCnt-- == 0) return DHT_ERROR_TIMEOUT; //55 ms?!

      
      loopCnt = DHT_LOOPS;
      while (digitalRead(configData.DHT22Pin) == LOW) if (loopCnt-- == 0) return DHT_ERROR_TIMEOUT;
      
      loopCnt = DHT_LOOPS;
      while (digitalRead(configData.DHT22Pin) == HIGH) if (loopCnt-- == 0) return DHT_ERROR_TIMEOUT;
      
      for (byte bitNum = 0; bitNum < 40; bitNum++) // try reading 40 bits
      {
        loopCnt = DHT_LOOPS;
        while (digitalRead(configData.DHT22Pin) == LOW) if (loopCnt-- == 0) return DHT_ERROR_TIMEOUT;
        
        delayMicroseconds(32);
        
        boolean dhtBit = digitalRead(configData.DHT22Pin);
        bitWrite(data[bitNum / 8], 7 - bitNum % 8, dhtBit);
        
        loopCnt = DHT_LOOPS;
        while (digitalRead(configData.DHT22Pin) == HIGH) if (loopCnt-- == 0) return DHT_ERROR_TIMEOUT;
      }
      sum = data[0] + data[1] + data[2] + data[3];
      if (data[4] != sum) return DHT_ERROR_CRC;

      humidity = data[0] * 256 + data[1];
      temperature = (data[2] & 0x7F) * 256 + data[3];
      if (data[2] & 0x80) temperature = -temperature;
      return DHT_OK;
    case DHT_TEMPERATURE:
      return temperature;
    case DHT_HUMIDITY:
      return humidity;
    case DHT_DATAPTR:
      return (int)data;
    default:
      return DHT_ERROR_UNKNOWN;
  }
}

void readDHTSensor(float &t, float &h)
{
  t = NONVALID_TEMPERATURE;
  h = NONVALID_HUMIDITY;
  switch ((dhtCall(DHT22_SAMPLE))) // always request a sample first
  {
    case DHT_OK: // only if DHT_OK is true, get temperature, humidity and possibly raw data
      t=dhtCall(DHT_TEMPERATURE)/10.0;
      h=dhtCall(DHT_HUMIDITY)/10.0;
      
      Serial.print("[!Hum:");
      Serial.print(h);
      Serial.println("]");
      Serial.print("[!Temp:");
      Serial.print(t);
      Serial.println("]");
      break;
    case DHT_ERROR_TIMEOUT:
      Serial.println(F("DHT Timeout"));
      break;
    case DHT_ERROR_CRC:
      Serial.println(F("DHT CRC"));
      break;
    default:
      Serial.println(F("DHT Unknown"));
  }
}



/*
 * Read values using DHTesp lib
 * 
 */
void printDHT(float &t, float &h)
{
  h = dht.getHumidity();
  t = dht.getTemperature();

  if ( isnan(h) ) h = NONVALID_HUMIDITY;
  if ( isnan(t) ) t = NONVALID_TEMPERATURE;

  if (h == NONVALID_HUMIDITY || t == NONVALID_TEMPERATURE) {
    Serial.print("[DHT] ");
    Serial.println(dht.getStatusString());
  }else{
    Serial.print("[!Hum:");
    Serial.print(h);
    Serial.println("]");
    Serial.print("[!Temp:");
    Serial.print(t);
    Serial.println("]");
  }
}
void printDHT_2(float &t, float &h)
{
  h = dht2.getHumidity();
  t = dht2.getTemperature();

  if ( isnan(h) ) h = NONVALID_HUMIDITY;
  if ( isnan(t) ) t = NONVALID_TEMPERATURE;

  if (h == NONVALID_HUMIDITY || t == NONVALID_TEMPERATURE) {
    Serial.print("[DHT2] ");
    Serial.println(dht2.getStatusString());
  }else{
    Serial.print("[!Hum2:");
    Serial.print(h);
    Serial.println("]");
    Serial.print("[!Temp2:");
    Serial.print(t);
    Serial.println("]");
  }
}
