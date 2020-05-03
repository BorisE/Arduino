float getOneWireTemp(uint8_t addr[8])
{
  byte i;
  byte present = 0;
  byte data[12];
  float celsius, fahrenheit;
  
  // start conversion
  oneWire.reset();
  oneWire.select(addr);
  oneWire.write(0x44,1);         
  delay(1000);     // maybe 750ms is enough, maybe not

  // Read Scratchpad
  present = oneWire.reset();
  oneWire.select(addr);    
  oneWire.write(0xBE);         

  // Read 9 bytes
  for ( i = 0; i < 9; i++) {           // we need 9 bytes
    data[i] = oneWire.read();
    //Serial.print(data[i], HEX);
    //Serial.print(" ");
  }
  //Serial.print(" CRC=");
  //Serial.print(OneWire::crc8(data, 8), HEX);
  //Serial.println();

  // Convert the data to actual temperature
  int16_t raw = ((int16_t)data[1] << 8) | data[0];
  byte cfg = (data[4] & 0x60); //4th byte is configuration byte
  if (cfg == 0x00) {
    raw = raw >> 3;  // 9 bit resolution, 93.75 ms
    celsius = (float)raw * 0.5;
    //Serial.print(" 9b ");
  }else if (cfg == 0x20) {
    raw = raw >> 2; // 10 bit res, 187.5 ms
    celsius = (float)raw * 0.25;
    //Serial.print(" 10b ");
  }else if (cfg == 0x40) {
    raw = raw >> 1; // 11 bit res, 375 ms
    celsius = (float)raw * 0.125;
    //Serial.print(" 11b ");
  }else{
  // default is 12 bit resolution, 750 ms conversion time
    celsius = (float)raw * 0.0625;
    //Serial.print(" 12b ");
  }

  return celsius;

}
