/*
 Based on sample code for the BH1750 Light sensor
 Version 0.1
*/
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int BH1750_Read(int address) //
{
  int i=0;
  Wire.beginTransmission(address);
  Wire.requestFrom(address, 2);
  while(Wire.available()) //
  {
    BH1750_buff[i] = Wire.read();  // receive one byte
    i++;
  }
  Wire.endTransmission();  
  return i;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Setting resolution mode
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void BH1750_Set_Resolution(int address, int resolution) 
{
  Wire.beginTransmission(address);
  Wire.write(resolution);
  Wire.endTransmission();

  BH1750_res=resolution;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Setting sensitivity
//Call set resolution mode after it
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void BH1750_Set_Sensitivity(int address, int sensitivity) 
{
  //Hi byte
  int hb=(sensitivity >> 5 | 0x40) ; //01000_MT[7,6,5]  0x40=01000_000
  Wire.beginTransmission(address);
  Wire.write(hb);
  Wire.endTransmission();

  //Lo byte
  int lb=((sensitivity <<3)>>3 | 0x60) ; //011_MT[4,3,2,1,0]  0x60=011_00000
  Wire.beginTransmission(address);
  Wire.write(lb);
  Wire.endTransmission();

  BH1750_sens=sensitivity;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void BH1750_Wait_time()
{
  float wait_time=BH1750_RES_1LX_WAITTIME;
  //wait
  switch (BH1750_res) {
    case BH1750_RES_1LX:
    case BH1750_RES_05LX:
       wait_time=BH1750_RES_1LX_WAITTIME;
      break;
    case BH1750_RES_4LX:
      wait_time=BH1750_RES_4LXL_WAITTIME;
      break;
  }
  wait_time=( wait_time * BH1750_sens / BH1750_SENS_DEFAULT + 20); //+20 just for sure
  BH1750_wait_time=int(wait_time);
 
  delay(BH1750_wait_time);
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
