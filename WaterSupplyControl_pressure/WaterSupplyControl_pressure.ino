/*
  Analog Input
  http://www.arduino.cc/en/Tutorial/AnalogInput
*/

int sensorPin = A0;    // select the input pin for the potentiometer
int sensorValue = 0;  // variable to store the value coming from the sensor
int correctionPedestal = 0;

void setup() {
  Serial.begin(115200);
  Serial.println("**********************************************");  
  // declare the ledPin as an OUTPUT:
  pinMode(LED_BUILTIN, OUTPUT);
}

void loop() {
  // read the value from the sensor:
  sensorValue = analogRead(sensorPin);
  
  float Vin = (sensorValue - correctionPedestal) * 3.3 / 1023.0;
  float pressVal = (Vin/3.3 - 0.1) * 4/3;
  
  Serial.print("ADC: ");
  Serial.print(sensorValue);
  Serial.print('\t');
  Serial.print("Vin: ");
  Serial.print(Vin);
  Serial.print('\t');
  Serial.print("P: ");
  Serial.print(pressVal);
  Serial.println();
  
  
  if (sensorValue > 100) {
    // turn the ledPin on
    digitalWrite(LED_BUILTIN, HIGH);
    delay(sensorValue);
    digitalWrite(LED_BUILTIN, LOW);
  }else{
    delay(250);
  }
  
}
