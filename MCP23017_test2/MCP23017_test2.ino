/**
 * On every loop, the state of the port B is copied to port A.
 *
 * Use active low inputs on port A. Internal pullups are enabled by default by the library so there is no need for external resistors.
 * Place LEDS on port B for instance. 
 * When pressing a button, the corresponding led is shut down.
 * 
 * You can also uncomment one line to invert the input (when pressing a button the corresponding led is lit)
 */
#include <Wire.h>
#include <MCP23017.h>

#define MCP23017_ADDR 0x22

#define TEST_WRITE_PIN 1
#define TEST_READ_PIN 2
MCP23017 mcp = MCP23017(MCP23017_ADDR);

void setup() {
    Wire.begin(D2,D1);
    Serial.begin(115200);
    
    mcp.init();
    //mcp.portMode(MCP23017Port::A, 0b11111111); //Port A as input
    mcp.portMode(MCP23017Port::A, 0); //Port A as input
    mcp.portMode(MCP23017Port::B, 0); //Port B as output


    //mcp.pinMode(TEST_READ_PIN,INPUT); // INPUT | OUTPUT
    mcp.pinMode(TEST_WRITE_PIN,OUTPUT); // INPUT | OUTPUT

    mcp.writeRegister(MCP23017Register::GPIO_A, 0x00);  //Reset port A 
    mcp.writeRegister(MCP23017Register::GPIO_B, 0x00);  //Reset port B

    // GPIO_B reflects the same logic as the input pins state
    //mcp.writeRegister(MCP23017Register::IPOL_B, 0x00);
    // Uncomment this line to invert inputs (press a button to lit a led)
    //mcp.writeRegister(MCP23017Register::IPOL_B, 0xFF);
}

void loop() {
    uint8_t currentB,currentW;

    /*currentB = mcp.digitalRead(TEST_READ_PIN);
    Serial.print ("Pin[");
    Serial.print (TEST_READ_PIN);
    Serial.print ("]: ");
    Serial.println(currentB);
    */
    
    for (int i=0; i<8; i++) {
      currentB = mcp.readPort(MCP23017Port::A);
      Serial.print ("Port A: ");
      Serial.println(currentB,BIN);
      delay (200);

      currentW = mcp.digitalRead(i);
      Serial.print ("Pin[");
      Serial.print (i);
      Serial.print ("]: ");
      Serial.println(currentW);
      delay (1000);
  
      if (currentW == 1){
        mcp.digitalWrite(i, LOW);
      }else{
        mcp.digitalWrite(i, HIGH);
      }
      delay (1000);
    }
}
