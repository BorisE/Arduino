float getOneWireTemp(uint8_t addr[8])
{
  byte data[12];
  byte present;
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
  if (cfg == 0x00) raw = raw & ~7;  // 9 bit resolution, 93.75 ms
  else if (cfg == 0x20) raw = raw & ~3; // 10 bit res, 187.5 ms
  else if (cfg == 0x40) raw = raw & ~1; // 11 bit res, 375 ms
  //// default is 12 bit resolution, no zeroing needed, 750 ms conversion time

  float temp = (float)raw / 16.0;

  return temp;
}
