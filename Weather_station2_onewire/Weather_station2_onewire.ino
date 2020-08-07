#include <OneWire.h>
#include <DallasTemperature.h>


#define ONE_WIRE_BUS D6 // Data wire is plugged into this port


#define NONVALID_TEMPERATURE -100


OneWire  ds(ONE_WIRE_BUS);  // on pin 10 (a 4.7K resistor is necessary)
//ROM = 28 6D A3 68 4 0 0 F8 
        
uint8_t OW_Temp1Addr[8] = { 0x28, 0x6D, 0xA3, 0x68, 0x4, 0x0, 0x0, 0xF8 };
uint8_t OW_Temp1Addr2[8] = { 0x28, 0x6D, 0xA3, 0x68, 0x4, 0x0, 0x0, 0xF8 };
float OW_Temp1=-100;

// Pass our oneWire reference to Dallas Temperature sensor 
DallasTemperature sensors(&ds);

void setup() {
  Serial.begin(115200);

  // Start the DS18B20 sensor
  sensors.begin();

}

void loop() {
  byte data[12];
  byte present = 0;
  byte i;
    
  ds.select(OW_Temp1Addr);

  ds.write(0x44, 1);        // start conversion, with parasite power on at the end
  delay(1000);     // maybe 750ms is enough, maybe not
  // we might do a ds.depower() here, but the reset will take care of it.
  
  present = ds.reset();
  
  ds.select(OW_Temp1Addr);    
  ds.write(0xBE);         // Read Scratchpad
  
  Serial.print("  Data = ");
  Serial.print(present, HEX);
  Serial.print(" ");
  for ( i = 0; i < 9; i++) {           // we need 9 bytes
    data[i] = ds.read();
    Serial.print(data[i], HEX);
    Serial.print(" ");
  }
  Serial.print(" CRC=");
  Serial.print(OneWire::crc8(data, 8), HEX);
  Serial.println();

  // Convert the data to actual temperature
  // because the result is a 16 bit signed integer, it should
  // be stored to an "int16_t" type, which is always 16 bits
  // even when compiled on a 32 bit processor.
  int16_t raw = (data[1] << 8) | data[0];
  byte cfg = (data[4] & 0x60);
  
  // at lower res, the low bits are undefined, so let's zero them
  if (cfg == 0x00) raw = raw & ~7;  // 9 bit resolution, 93.75 ms
  else if (cfg == 0x20) raw = raw & ~3; // 10 bit res, 187.5 ms
  else if (cfg == 0x40) raw = raw & ~1; // 11 bit res, 375 ms
  //// default is 12 bit resolution, no zeroing needed, 750 ms conversion time

  OW_Temp1 = (float)raw / 16.0;
  Serial.print("  Temperature = ");
  Serial.print(OW_Temp1);
  Serial.println(" C");

  OW_Temp1 = getOneWireTemp(OW_Temp1Addr);
  Serial.print("  Temperature = ");
  Serial.print(OW_Temp1);
  Serial.println(" C");

  delay(1000);
  //sensors.requestTemperatures(); 
  float temperatureC = sensors.getTempCByIndex(0);
  Serial.print("Using Dallas lib: ");
  Serial.print(temperatureC);
  Serial.println("ºC");

  Serial.print("Using Dallas lib2: count=");
  Serial.println(sensors.getDeviceCount());
  sensors.getAddress(OW_Temp1Addr2,0);
  printAddress(OW_Temp1Addr2);
  Serial.println();
  
  float OW_Temp1=NONVALID_TEMPERATURE;
  if (sensors.getAddress(OW_Temp1Addr2, 0))
  {
    Serial.println(" here ");
    OW_Temp1 = sensors.getTempC(OW_Temp1Addr2);
  }else{
     OW_Temp1 = NONVALID_TEMPERATURE;
  }
  Serial.print(OW_Temp1);
  Serial.println("ºC");
  
  Serial.print(temperatureC);
  Serial.println("ºC");
  delay(1000);

}


float getOneWireTemp(uint8_t addr[8])
{
  byte data[12];
  byte present = 0;
  byte i;
    
  ds.select(addr);

  // start conversion, with parasite power on at the end
  ds.write(0x44, 1);        
  delay(1000);     // maybe 750ms is enough, maybe not
  
  // we might do a ds.depower() here, but the reset will take care of it.
  present = ds.reset();
  
  // Read Scratchpad
  ds.select(addr);    
  ds.write(0xBE);         
  
  //Serial.print("  Data = ");
  //Serial.print(present, HEX);
  //Serial.print(" | ");
  for ( i = 0; i < 9; i++) {           // we need 9 bytes
    data[i] = ds.read();
    //Serial.print(data[i], HEX);
    //Serial.print(" ");
  }
  //Serial.print(" CRC=");
  //Serial.print(OneWire::crc8(data, 8), HEX);
  //Serial.println();

  // Convert the data to actual temperature
  // because the result is a 16 bit signed integer, it should
  // be stored to an "int16_t" type, which is always 16 bits
  // even when compiled on a 32 bit processor.
  int16_t raw = (data[1] << 8) | data[0];
  byte cfg = (data[4] & 0x60);
  
  // at lower res, the low bits are undefined, so let's zero them
  if (cfg == 0x00){
    raw = raw & ~7;  // 9 bit resolution, 93.75 ms
    Serial.print (" 9b ");
  }
  else if (cfg == 0x20){
    raw = raw & ~3; // 10 bit res, 187.5 ms
    Serial.print (" 10b ");
  }
  else if (cfg == 0x40){
    raw = raw & ~1; // 11 bit res, 375 ms
    Serial.print (" 11b ");
  }
  else
  {
    Serial.print (" 12b ");
  }
  //// default is 12 bit resolution, no zeroing needed, 750 ms conversion time

  return (float)raw / 16.0;
}

void printAddress(uint8_t addr[8])
{
  for ( uint8_t i = 0; i < 8; i++) {           // we need 8 bytes
    Serial.print(addr[i], HEX);
    Serial.print(" ");
}

}
