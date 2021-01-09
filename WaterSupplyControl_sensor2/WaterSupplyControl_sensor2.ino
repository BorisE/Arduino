#define WSPin D2
#define WSModePin D1

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);

  pinMode(WSPin, INPUT);
  pinMode(WSModePin, OUTPUT);
  digitalWrite(WSModePin,  HIGH);
}

void loop() {
  // put your main code here, to run repeatedly:
  static int WS = digitalRead(WSPin);
  Serial.print("WS = ");
  Serial.println(WS);
  delay (500);
}
