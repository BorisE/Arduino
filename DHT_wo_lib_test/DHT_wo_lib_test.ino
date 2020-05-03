// DHT11 and DHT22 sensor reading demo
// by 'jurs' for Arduino Forum
// https://forum.arduino.cc/index.php?topic=308644.msg2142725#msg2142725

// DHT functions enumerated
enum {DHT22_SAMPLE, DHT_TEMPERATURE, DHT_HUMIDITY, DHT_DATAPTR};

// Begin of user configuration area
#define DHT_PIN 4
// End of user configuration area

// DHT error codes enumerated
enum {DHT_OK = 0, DHT_ERROR_TIMEOUT = -1, DHT_ERROR_CRC = -2, DHT_ERROR_UNKNOWN = -3};

// DHT sensor pinout from left to right looking at the gridded side
// 1-VCC  2-DATA  3-NC  4-GND

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
      pinMode(DHT_PIN, INPUT_PULLUP);
      loopCnt = DHT_LOOPS;
      while (digitalRead(DHT_PIN) == HIGH) if (loopCnt-- == 0) return DHT_ERROR_TIMEOUT;
      loopCnt = DHT_LOOPS;
      while (digitalRead(DHT_PIN) == LOW) if (loopCnt-- == 0) return DHT_ERROR_TIMEOUT;
      loopCnt = DHT_LOOPS;
      while (digitalRead(DHT_PIN) == HIGH) if (loopCnt-- == 0) return DHT_ERROR_TIMEOUT;
      for (byte bitNum = 0; bitNum < 40; bitNum++) // try reading 40 bits
      {
        loopCnt = DHT_LOOPS;
        while (digitalRead(DHT_PIN) == LOW) if (loopCnt-- == 0) return DHT_ERROR_TIMEOUT;
        delayMicroseconds(32);
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


void setup()
{
  // initialize serial for debugging
  Serial.begin(9600);
}

void loop()
{
  byte* b; // byte pointer for showing raw data
  switch ((dhtCall(DHT22_SAMPLE))) // always request a sample first
  {
    case DHT_OK: // only if DHT_OK is true, get temperature, humidity and possibly raw data
      Serial.print("Humidity: ");
      Serial.print(dhtCall(DHT_HUMIDITY));
      Serial.print("   Temp: ");
      Serial.print(dhtCall(DHT_TEMPERATURE));
      Serial.print("     raw data:  ");
      b = (byte*)dhtCall(DHT_DATAPTR);
      for (int i = 0; i < 5; i++)
      {
        Serial.print(b[i]); Serial.print('\t');
      }
      Serial.println();
      break;
    case DHT_ERROR_TIMEOUT:
      Serial.println("Timeout Error");
      break;
    case DHT_ERROR_CRC:
      Serial.println("CRC Error");
      break;
    default:
      Serial.println("Unknown Error");
  }
  delay(2000); // minimum time between two DHT samples is two seconds
}
