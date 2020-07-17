// Connect pin 1 (on the left) of the sensor to +5V
// Connect pin 2 of the sensor to whatever your DHTPIN is
// Connect pin 4 (on the right) of the sensor to GROUND
// Connect a 10K resistor from pin 2 (data) to pin 1 (power) of the sensor

#define DHT_PIN D8

enum {DHT22_SAMPLE, DHT_TEMPERATURE, DHT_HUMIDITY, DHT_DATAPTR};  // DHT functions enumerated
enum {DHT_OK = 0, DHT_ERROR_TIMEOUT = -1, DHT_ERROR_CRC = -2, DHT_ERROR_UNKNOWN = -3};  // DHT error codes enumerated
float dhtTemp = -100;
float dhtHum =0;
unsigned long _lastReadTime_DHT=0;


void setup() {
  Serial.begin(9600);
  Serial.setTimeout(2000);

  // Wait for serial to initialize.
  while(!Serial) { }
   
  Serial.println("DHT22 wolib test!");
 
}

void loop() {

    readDHTSensor(dhtTemp, dhtHum);

    Serial.print("Humidity: "); 
    Serial.print(dhtHum);
    Serial.print(" %\t");
    Serial.print("Temperature: "); 
    Serial.print(dhtTemp);
    Serial.println(" *C");
}


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
#define DHT_LOOPS 1800
  int  triggerTime = 1000; // 1000µs trigger time for DHT22/ 2000 fir DHT11
  switch (function)
  {
    case DHT22_SAMPLE: // REQUEST DHT22 SAMPLE
      
      pinMode(DHT_PIN, OUTPUT);
      digitalWrite(DHT_PIN, LOW);
      
      delayMicroseconds(triggerTime);
      //delay (1);

      pinMode(DHT_PIN, INPUT_PULLUP);
      loopCnt = DHT_LOOPS;
      while (digitalRead(DHT_PIN) == HIGH){
        if (loopCnt-- == 0) return DHT_ERROR_TIMEOUT;
        delayMicroseconds(1);
      }
      loopCnt = DHT_LOOPS;
      while (digitalRead(DHT_PIN) == LOW){
        if (loopCnt-- == 0) return DHT_ERROR_TIMEOUT;
        delayMicroseconds(1);
      }
      loopCnt = DHT_LOOPS;
      while (digitalRead(DHT_PIN) == HIGH){
        if (loopCnt-- == 0) return DHT_ERROR_TIMEOUT;
        delayMicroseconds(1);
      }

      // try reading 40 bits
      for (byte bitNum = 0; bitNum < 40; bitNum++) 
      {
        loopCnt = DHT_LOOPS;
        while (digitalRead(DHT_PIN) == LOW) if (loopCnt-- == 0) return DHT_ERROR_TIMEOUT;
        
        delayMicroseconds(32);
        //delay(1);
        
        boolean dhtBit = digitalRead(DHT_PIN);
        bitWrite(data[bitNum / 8], 7 - bitNum % 8, dhtBit);
        loopCnt = DHT_LOOPS;
        while (digitalRead(DHT_PIN) == HIGH) if (loopCnt-- == 0) return DHT_ERROR_TIMEOUT;
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
  byte* b; // byte pointer for showing raw data
  switch ((dhtCall(DHT22_SAMPLE))) // always request a sample first
  {
    case DHT_OK: // only if DHT_OK is true, get temperature, humidity and possibly raw data
      t=dhtCall(DHT_TEMPERATURE)/10.0;
      h=dhtCall(DHT_HUMIDITY)/10.0;
      
      //Serial.print("[!Hum:");
      //Serial.print(h);
      //Serial.println("]");
      //Serial.print("[!Temp:");
      //Serial.print(t);
      //Serial.println("]");
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
