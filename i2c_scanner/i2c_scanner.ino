/*
 * 
 * СКАНЕР I2C шины
 * 
 * Дополнение кода для проверки ESP8266
 * Подключение - пин GPIO5 - SCK, GPIO4- SDA (На пинах GPIO14 и GPIO2 не работало
 * Надо было определить эти пины в setup
 * 
 * Проверено на ESP-07 и DS3231, BMP280
 * Wemos D1 R1: на пинах D3 SDA/D4 SCK и BME280
 * 
 */

/* for Wemos D1 R2
 *  
#define PIN_WIRE_SDA (D1)    4   (by default, but I redefined, see below)
#define PIN_WIRE_SCL (D2)    5   (by default, but I redefined, see below)

//new ESP-12E GPIO2
#define LED_BUILTIN 2       D4

static const uint8_t D0   = 16; /no interrupts                    <-relay
static const uint8_t D1   = 5;  /SCL     
static const uint8_t D2   = 4;  /SDA
static const uint8_t D3   = 0;  /10k pullup                       <-always low on start
static const uint8_t D4   = 2;  /Tx1, 10k pullup, Buit-in LED     <-яркий диод
static const uint8_t D5   = 14; /built in led2 ("SCK")            <-relay
static const uint8_t D6   = 12;                                   <-relay
static const uint8_t D7   = 13; /RX0* 10k pulldown                <-relay
static const uint8_t D8   = 15; /Tx0*                             <-при загузке скетча использует этот пин. потом можно использовать; для реле не подходит

GPIO6-GPIO11 - flash pins
*/
#include <Wire.h>


void setup()
{
  //Wire.begin();       // SCK - GPIO5,  SDA - GPIO4
  //Wire.begin(2,14);   // SCK - GPIO14, SDA - GPIO2
  Wire.begin();    // SCK - D4 (wemos d1 r1 GPIO4), SDA - D3 (wemos d1 r1 GPIO5)

  Serial.begin(115200);
  Serial.println("\nI2C Scanner");
  
  //pinMode(LED_BUILTIN, OUTPUT);
  //digitalWrite(LED_BUILTIN, LOW);   // turn the LED on (HIGH is the voltage level)
  
}


void loop()
{
  byte error, address;
  int nDevices;

  Serial.println("Scanning...");

  nDevices = 0;
  for(address = 1; address < 127; address++ )
  {
    // The i2c_scanner uses the return value of
    // the Write.endTransmisstion to see if
    // a device did acknowledge to the address.

    
    Wire.beginTransmission(address);
    error = Wire.endTransmission();

    if (error == 0)
    {
      Serial.print("I2C device found at address 0x");
      if (address<16)
        Serial.print("0");
      Serial.print(address,HEX);
      Serial.println("  !");

      nDevices++;
    }
    else if (error==4)
    {
      Serial.print("Unknow error at address 0x");
      if (address<16)
        Serial.print("0");
      Serial.println(address,HEX);
    }    
  }
  if (nDevices == 0)
    Serial.println("No I2C devices found\n");
  else
    Serial.println("done\n");

  delay(2000);           // wait 5 seconds for next scan
}
