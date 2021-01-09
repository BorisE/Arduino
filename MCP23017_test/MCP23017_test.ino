#include <Wire.h>
#include "Adafruit_MCP23017.h"

// НЕ УДАЛОСЬ ЗАПУСТИТЬ!!!

// Basic pin reading and pullup test for the MCP23017 I/O expander
// public domain!

// Connect pin #12 of the expander to Analog 5 (i2c clock)
// Connect pin #13 of the expander to Analog 4 (i2c data)
// Connect pins #15, 16 and 17 of the expander to ground (address selection)
// Connect pin #9 of the expander to 5V (power)
// Connect pin #10 of the expander to ground (common ground)
// Connect pin #18 through a ~10kohm resistor to 5V (reset pin, active low)

// Input #0 is on pin 21 so connect a button or switch from there to ground
/*
 * Adafruit PIN  Pin Name  Physical Pin
0   GPA0  21
1   GPA1  22
2   GPA2  23
3   GPA3  24
4   GPA4  25
5   GPA5  26
6   GPA6  27
7   GPA7  28
8   GPB0  1
9   GPB1  2
10  GPB2  3
11  GPB3  4
12  GPB4  5
13  GPB5  6
14  GPB6  7
15  GPB7  8



Address   Adafruit address  A0 / PIN 15   A1 / PIN 16   A2 / PIN 17
0x20  0   LOW   LOW   LOW
0x21  1   HIGH  LOW   LOW
0x22  2   LOW   HIGH  LOW
0x23  3   HIGH  HIGH  LOW
0x24  4   LOW   LOW   HIGH
0x25  5   HIGH  LOW   HIGH
0x26  6   LOW   HIGH  HIGH
0x27  7   HIGH  HIGH  HIGH
 */
Adafruit_MCP23017 mcp;

#define MCP_ADDR 0x21
#define MCP_testpin 8
#define MCP_testpin2 9

#define testReadPin D3
void setup() {  
  Serial.begin(115200);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB
  }
  Wire.begin(D2,D1);
  
  mcp.begin(MCP_ADDR);

  mcp.pinMode(MCP_testpin, INPUT);
  mcp.pinMode(MCP_testpin2, OUTPUT);
  //mcp.pullUp(MCP_testpin, LOW);  // turn on a 100K pullup internally

  pinMode(testReadPin, INPUT);

  //test i2c device is present
  Serial.println("****");
  Serial.println();
  byte error;
  Wire.beginTransmission(MCP_ADDR);
  error = Wire.endTransmission();

  if (error == 0)
  {
    Serial.print("I2C device found at address 0x");
    if (MCP_ADDR<16)
      Serial.print("0");
    Serial.print(MCP_ADDR,HEX);
    Serial.println("  !");

  }
  else if (error==4)
  {
    Serial.print("Unknow error at address 0x");
    if (MCP_ADDR<16)
      Serial.print("0");
    Serial.println(MCP_ADDR,HEX);
  }    
}



void loop() {
  // The LED will 'echo' the button
  //digitalWrite(LED_BUILTIN, mcp.digitalRead(MCP_testpin));
  Serial.print("Pin[");
  Serial.print(MCP_testpin);
  Serial.print("] val:");
  Serial.println(mcp.digitalRead(MCP_testpin));
  delay(200);
  
  mcp.digitalWrite(MCP_testpin2,LOW);
  delay(50);
  Serial.print("Read[");
  Serial.print(MCP_testpin2);
  Serial.print("] val:");
  Serial.println(digitalRead(testReadPin));
  delay(200);

  mcp.digitalWrite(MCP_testpin2,HIGH);
  delay(50);
  Serial.print("Read[");
  Serial.print(MCP_testpin2);
  Serial.print("] val:");
  Serial.println(digitalRead(testReadPin));
  delay(200);
}
