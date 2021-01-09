#include <Wire.h>
#include <MCP23017.h>

#define MCP23017_ADDR 0x22
MCP23017 mcp = MCP23017(MCP23017_ADDR);

#define H1_1_PIN_DEFAULT 6
#define H1_2_PIN_DEFAULT 7
#define CLOSED_PIN 10
#define OPENED_PIN 10

unsigned long currenttime=0;              // millis from script start 
unsigned long _lastReadTime_WS=0;
#define WSENSORS_READ_INTERVAL 15000

const int dir = 0; // 1 -OPEN, 0 - CLOSED

void setup() {
  Serial.begin(115200);
  Serial.println("**********************************************");  
  
  Wire.begin(D2,D1);
  mcp.init();
  mcp.portMode(MCP23017Port::A, 0); //Port A as output
  mcp.portMode(MCP23017Port::B, 0b11111111); //Port B as input

  mcp.writeRegister(MCP23017Register::GPIO_A, 0x00);  //Reset port A 
  mcp.writeRegister(MCP23017Register::GPIO_B, 0x00);  //Reset port B


  initRelays();

  mcp.pinMode(CLOSED_PIN, INPUT);
  
  //mcp.pinMode(OPENED_PIN, INPUT_PULLUP);
  mcp.pinMode(OPENED_PIN, INPUT);

}

void loop() {
  currenttime = millis();

  int val = Serial.read() - '0';
  if (val == 1) 
  {
    OpenVent();
  }
  else if (val == 0) 
  {
    CloseVent();
  }
  
   // Every second, calculate and print litres/hour
   if(_lastReadTime_WS ==0 || (currenttime - _lastReadTime_WS) > WSENSORS_READ_INTERVAL)
   {
      Serial.print("[H1_1: ");
      Serial.print(mcp.digitalRead(H1_1_PIN_DEFAULT));
      Serial.println("]");
      Serial.print("[H1_2: ");
      Serial.print(mcp.digitalRead(H1_2_PIN_DEFAULT));
      Serial.println("]");
      Serial.println();
      _lastReadTime_WS = currenttime;
   }

  int ClosedP=mcp.digitalRead(CLOSED_PIN);
  int OpenedP=mcp.digitalRead(OPENED_PIN);
  Serial.print("[Closed pin: ");
  Serial.print(ClosedP);
  Serial.print("] ");

  Serial.print("[Opened pin: ");
  Serial.print(OpenedP);
  Serial.println("]");

  if (ClosedP == 1 && OpenedP == 1) {
    Serial.println("State: CLOSED");
  }else if (ClosedP == 0 && OpenedP == 0) {
    Serial.println("State: OPENED");
  }else {
    Serial.println("State: MOVING");
  }
  

  delay(500);
}


void initRelays()
{
  mcp.pinMode(H1_1_PIN_DEFAULT, OUTPUT);    // Установим вывод 5 как выход
  mcp.pinMode(H1_2_PIN_DEFAULT, OUTPUT);    // Установим вывод 6 как выход

  //
  if (dir == 1) {
    // OPEN
    OpenVent();
  }else{
    // CLOSE
    CloseVent();
  }
}
void OpenVent() {
    Serial.println("OPENINIG");
    mcp.digitalWrite(H1_1_PIN_DEFAULT, LOW); // Выключаем реле
    mcp.digitalWrite(H1_2_PIN_DEFAULT, HIGH); // Выключаем реле
}
void CloseVent() {
    Serial.println("CLOSING");
    mcp.digitalWrite(H1_1_PIN_DEFAULT, HIGH); // Выключаем реле
    mcp.digitalWrite(H1_2_PIN_DEFAULT, LOW); // Выключаем реле
}
