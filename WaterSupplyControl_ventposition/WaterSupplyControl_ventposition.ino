#define H1_1_PIN_DEFAULT D7
#define H1_2_PIN_DEFAULT D6
#define CLOSED_PIN D4
#define OPENED_PIN D5

unsigned long currenttime=0;              // millis from script start 
unsigned long _lastReadTime_WS=0;
#define WSENSORS_READ_INTERVAL 15000

const int dir = 0; // 1 -OPEN, 0 - CLOSED

void setup() {
  Serial.begin(115200);
  Serial.println("**********************************************");  
  initRelays();

  pinMode(CLOSED_PIN, INPUT);
  
  pinMode(OPENED_PIN, INPUT);
  digitalWrite(OPENED_PIN,HIGH);
  
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
      Serial.print(digitalRead(H1_1_PIN_DEFAULT));
      Serial.println("]");
      Serial.print("[H1_2: ");
      Serial.print(digitalRead(H1_2_PIN_DEFAULT));
      Serial.println("]");
      Serial.println();
      _lastReadTime_WS = currenttime;
   }

  Serial.print("[Closed: ");
  Serial.print(digitalRead(CLOSED_PIN));
  Serial.print("] ");

  Serial.print("[Opened: ");
  Serial.print(digitalRead(OPENED_PIN));
  Serial.println("]");


  delay(500);
}


void initRelays()
{
  pinMode(H1_1_PIN_DEFAULT, OUTPUT);    // Установим вывод 5 как выход
  pinMode(H1_2_PIN_DEFAULT, OUTPUT);    // Установим вывод 6 как выход

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
    digitalWrite(H1_1_PIN_DEFAULT, LOW); // Выключаем реле
    digitalWrite(H1_2_PIN_DEFAULT, HIGH); // Выключаем реле
}
void CloseVent() {
    Serial.println("CLOSING");
    digitalWrite(H1_1_PIN_DEFAULT, HIGH); // Выключаем реле
    digitalWrite(H1_2_PIN_DEFAULT, LOW); // Выключаем реле
}
