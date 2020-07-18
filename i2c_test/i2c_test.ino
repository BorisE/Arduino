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

#include <Wire.h>


void setup()
{
  //Wire.begin();       // SCK - GPIO5,  SDA - GPIO4
  //Wire.begin(2,14);   // SCK - GPIO14, SDA - GPIO2
  Wire.begin(D3,D4);    // SCK - D4 (wemos d1 r1 GPIO4), SDA - D3 (wemos d1 r1 GPIO5)

  Serial.begin(115200);
  Serial.println("\nI2C Scanner");
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

  delay(5000);           // wait 5 seconds for next scan
}
