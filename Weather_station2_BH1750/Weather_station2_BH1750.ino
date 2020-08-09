/*

  Example of BH1750 library usage.

  This example initialises the BH1750 object using the default high resolution
  one shot mode and then makes a light level reading every five seconds.

  After the measurement the MTreg value is changed according to the result:
  lux > 40000 ==> MTreg =  32
  lux < 40000 ==> MTreg =  69  (default)
  lux <    10 ==> MTreg = 138
  Remember to test your specific sensor! Maybe the MTreg value range from 32
  up to 254 is not applicable to your unit.

  Connection:

    VCC -> 3V3 or 5V
    GND -> GND
    SCL -> SCL (A5 on Arduino Uno, Leonardo, etc or 21 on Mega and Due, on esp8266 free selectable)
    SDA -> SDA (A4 on Arduino Uno, Leonardo, etc or 20 on Mega and Due, on esp8266 free selectable)
    ADD -> (not connected) or GND

  ADD pin is used to set sensor I2C address. If it has voltage greater or equal to
  0.7VCC voltage (e.g. you've connected it to VCC) the sensor address will be
  0x5C. In other case (if ADD voltage less than 0.7 * VCC) the sensor address will
  be 0x23 (by default).

*/

#include <Wire.h>
#include <BH1750.h>

//I2C wire
#define SDA_PIN_DEFAULT D3
#define SCL_PIN_DEFAULT D4

#define BH1750_ADDR 0x23        // if ADD pin to GROUND, 0x23; if to VCC - 0x5C
BH1750 lightMeter (BH1750_ADDR);

#define BH1750_HIGHLUM_THRESHOLD 40000.0
#define BH1750_LOWLUM_THRESHOLD 10.0


void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);

  Wire.begin(SDA_PIN_DEFAULT, SCL_PIN_DEFAULT);
  lightMeter.begin(BH1750::CONTINUOUS_HIGH_RES_MODE);
  Serial.println(F("BH1750 Test begin in CONTINUOUS_HIGH_RES_MODE"));
}



float measureLight()
{
  //we use here the maxWait option due fail save
  float lux = lightMeter.readLightLevel(true);
//  Serial.print(F("Light: "));
//  Serial.print(lux);
//  Serial.println(F(" lx"));
  if (lux < 0) {
    Serial.println(F("[BH1750] Error condition detected"));
  }
  else {
    Serial.print("[!BH:");
    Serial.print(lux);
    Serial.println("]");

    if (lux > BH1750_HIGHLUM_THRESHOLD) {
      // reduce measurement time - needed in direct sun light
      if (lightMeter.setMTreg(32)) {
        Serial.println(F("[BH1750] Setting MTReg to low value for high light environment"));
      }
      else {
        Serial.println(F("[BH1750] Error setting MTReg to low value for high light environment"));
      }
    }
    else {
        if (lux > BH1750_LOWLUM_THRESHOLD) {
          // typical light environment
          if (lightMeter.setMTreg(69)) {
            Serial.println(F("[BH1750] Setting MTReg to default value for normal light environment"));
          }
          else {
            Serial.println(F("[BH1750] Error setting MTReg to default value for normal light environment"));
          }
        }
        else {
          if (lux <= BH1750_LOWLUM_THRESHOLD) {
            //very low light environment
            if (lightMeter.setMTreg(138)) {
              Serial.println(F("[BH1750] Setting MTReg to high value for low light environment"));
            }
            else {
              Serial.println(F("[BH1750] Error setting MTReg to high value for low light environment"));
            }
          }
       }
    }
  }

  return lux;
}

int MTregVal = 31;
int measTime =0;
int modeNum=0;
int baseDur = 120;

void loop() {
  //float lux = measureLight();

  MTregVal+=5;
  if (MTregVal>254) {
    MTregVal=31;
    modeNum++;
    if (modeNum>5) modeNum = 0;

    switch (modeNum)
    {
      case 0: 
        lightMeter.configure(BH1750::CONTINUOUS_HIGH_RES_MODE);
        Serial.println("CONTINUOUS_HIGH_RES_MODE");
        baseDur = 120;
        break;
      case 1: 
        lightMeter.configure(BH1750::CONTINUOUS_HIGH_RES_MODE_2);
        Serial.println("CONTINUOUS_HIGH_RES_MODE_2");
        baseDur = 120;
        break;
      case 2: 
        lightMeter.configure(BH1750::CONTINUOUS_LOW_RES_MODE);
        Serial.println("CONTINUOUS_LOW_RES_MODE");
        baseDur = 16;
        break;
      case 3: 
        lightMeter.configure(BH1750::ONE_TIME_HIGH_RES_MODE);
        Serial.println("ONE_TIME_HIGH_RES_MODE");
        baseDur = 120;
        break;
      case 4: 
        lightMeter.configure(BH1750::ONE_TIME_HIGH_RES_MODE_2);
        Serial.println("ONE_TIME_HIGH_RES_MODE_2");
        baseDur = 120;
        break;
      case 5: 
        lightMeter.configure(BH1750::ONE_TIME_LOW_RES_MODE);
        Serial.println("ONE_TIME_LOW_RES_MODE");
        baseDur = 16;
        break;
    }  
  }

  measTime= int(baseDur * MTregVal / 69.0 + 30.0);

  if (lightMeter.setMTreg(MTregVal)) {
  }
  else {
    Serial.println(F("[BH1750] Error setting MTReg to low value for high light environment"));
  }
  //we use here the maxWait option due fail save
  float lux = lightMeter.readLightLevel(true);
  Serial.print(F("Light: "));
  Serial.print(lux);
  Serial.print(F(" lx | "));

  Serial.print("MTreg: ");
  Serial.print(MTregVal);
  Serial.print("   -> delay:");
  Serial.println(measTime);

  delay(measTime);
}
