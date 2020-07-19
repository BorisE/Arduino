/***************************************************************************
  This is a library for the BME280 humidity, temperature & pressure sensor

  Designed specifically to work with the Adafruit BME280 Breakout
  ----> http://www.adafruit.com/products/2650

  These sensors use I2C or SPI to communicate, 2 or 4 pins are required
  to interface. The device's I2C address is either 0x76 or 0x77.

  Adafruit invests time and resources providing this open source code,
  please support Adafruit andopen-source hardware by purchasing products
  from Adafruit!

  Written by Limor Fried & Kevin Townsend for Adafruit Industries.
  BSD license, all text above must be included in any redistribution
  See the LICENSE file for details.
 ***************************************************************************/

#include <Wire.h>
#include <BME280_I2C.h>

#define BME280_ADDRESS (0x76)
#define SEALEVELPRESSURE_HPA (1013.25)

// Create BME280 object
BME280_I2C bme(0x76); // I2C using address 0x77

unsigned long delayTime;

void setup() {
    Serial.begin(115200);
    while(!Serial);    // time to get serial running
    Serial.println(F("BME280 test"));

    if (!bme.begin(D3, D4)) 
    {
      Serial.println("Could not find a valid BME280 sensor, check wiring!");
      while (1);
    } 
    
    Serial.println("-- Default Test --");
    delayTime = 1000;

    Serial.println();
}


void loop() { 
    //printValues();
    //readWOlib();
    printValuesNewLib();
    delay(delayTime);
}


void printValuesNewLib()
{
  bme.readSensor();
  Serial.print(bme.getPressure_MB()); Serial.print(" mb\t"); // Pressure in millibars
  Serial.print(bme.getPressure_P()); Serial.print(" p\t"); // Pressure in pascals
  Serial.print(bme.getPressure_atm()); Serial.print(" atm\t"); // Pressure in atm
  Serial.print(bme.getPressure_mmHg()); Serial.print(" mmHg\t"); // Pressure in mmHg
  Serial.print(bme.getHumidity()); Serial.print(" %\t\t");
  Serial.print(bme.getTemperature_C()); Serial.print(" *C\t");
  Serial.print(bme.getTemperature_F()); Serial.println(" *F"); 
}
