#define BH1750_HIGHLUM_THRESHOLD 40000.0
#define BH1750_LOWLUM_THRESHOLD 10.0

#define BH1750_HIGHLIGHT_MTREG_VAL  32
#define BH1750_NORMLIGHT_MTREG_VAL  69
#define BH1750_LOWLIGHT_MTREG_VAL   138

int BH1750_currentMode = BH1750::ONE_TIME_HIGH_RES_MODE;
int BH1750_currentMTreg = 69;


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
    Serial.print("[!BHV:");
    Serial.print(lux);
    Serial.println("]");

    //very high light enviroment
    if (lux > BH1750_HIGHLUM_THRESHOLD) {
      if (BH1750_currentMTreg != BH1750_HIGHLIGHT_MTREG_VAL) {
        // reduce measurement time - needed in direct sun light
        if (lightMeter.setMTreg(BH1750_HIGHLIGHT_MTREG_VAL)) {
          Serial.println(F("[BH1750] Setting MTReg to low value for high light environment"));
          BH1750_currentMTreg=BH1750_HIGHLIGHT_MTREG_VAL;
        }
        else {
          Serial.println(F("[BH1750] Error setting MTReg to low value for high light environment"));
        }
      }
        
      // Change resolution to low
      if (BH1750_currentMode != BH1750::ONE_TIME_LOW_RES_MODE){
        if (lightMeter.configure(BH1750::ONE_TIME_LOW_RES_MODE)){
          Serial.println(F("[BH1750] Setting MODE to ONE_TIME_LOW_RES_MODE"));
          BH1750_currentMode = BH1750::ONE_TIME_LOW_RES_MODE;
        }
        else {
          Serial.println(F("[BH1750] Error setting MODE to ONE_TIME_LOW_RES_MODE"));
        }
      }
    }
    //very low light environment
    else if (lux <= BH1750_LOWLUM_THRESHOLD) {
      if (BH1750_currentMTreg != BH1750_LOWLIGHT_MTREG_VAL) {
        if (lightMeter.setMTreg(BH1750_LOWLIGHT_MTREG_VAL)) {
          Serial.println(F("[BH1750] Setting MTReg to high value for low light environment"));
          BH1750_currentMTreg = BH1750_LOWLIGHT_MTREG_VAL;
        }
        else {
          Serial.println(F("[BH1750] Error setting MTReg to high value for low light environment"));
        }
      }
      // Change resolution high2
      if (BH1750_currentMode != BH1750::ONE_TIME_HIGH_RES_MODE_2){
        if (lightMeter.configure(BH1750::ONE_TIME_HIGH_RES_MODE_2)){
          Serial.println(F("[BH1750] Setting MODE to ONE_TIME_HIGH_RES_MODE_2"));
          BH1750_currentMode = BH1750::ONE_TIME_HIGH_RES_MODE_2;
        }
        else {
          Serial.println(F("[BH1750] Error setting MODE to ONE_TIME_HIGH_RES_MODE_2"));
        }
      }
    }
    else {
      // typical light environment
      if (BH1750_currentMTreg != BH1750_NORMLIGHT_MTREG_VAL) {
        if (lightMeter.setMTreg(BH1750_NORMLIGHT_MTREG_VAL)) {
          Serial.println(F("[BH1750] Setting MTReg to default value for normal light environment"));
          BH1750_currentMTreg = BH1750_NORMLIGHT_MTREG_VAL;
        }
        else {
          Serial.println(F("[BH1750] Error setting MTReg to default value for normal light environment"));
        }
      }
      // Change resolution high
      if (BH1750_currentMode != BH1750::ONE_TIME_HIGH_RES_MODE){
        if (lightMeter.configure(BH1750::ONE_TIME_HIGH_RES_MODE)){
          Serial.println(F("[BH1750] Setting MODE to ONE_TIME_HIGH_RES_MODE"));
          BH1750_currentMode = BH1750::ONE_TIME_HIGH_RES_MODE;
        }
        else {
          Serial.println(F("[BH1750] Error setting MODE to ONE_TIME_HIGH_RES_MODE"));
        }
      }
    }
  }

  return lux;
}
