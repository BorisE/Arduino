/*
  Button

  Turns on and off a light emitting diode(LED) connected to digital pin 13,
  when pressing a pushbutton attached to pin 2.

  The circuit:
  - LED attached from pin 13 to ground
  - pushbutton attached to pin 2 from +5V
  - 10K resistor attached to pin 2 from ground

  - Note: on most Arduinos there is already an LED on the board
    attached to pin 13.

  created 2005
  by DojoDave <http://www.0j0.org>
  modified 30 Aug 2011
  by Tom Igoe

  This example code is in the public domain.

  http://www.arduino.cc/en/Tutorial/Button
*/

// constants won't change. They're used here to set pin numbers:
const int buttonPin = 3;     // the number of the pushbutton pin (UNO: only 2 or 3)


// variables will change:
volatile byte buttonState = LOW;

void setup() {
    Serial.begin(9600);
  // initialize the pushbutton pin as an input:
  pinMode(buttonPin, INPUT);

  attachInterrupt(digitalPinToInterrupt(buttonPin), ReadButtonPress, RISING);
}

void ReadButtonPress()
{
  static unsigned long lastread;
  if ((millis() - lastread) > 1000)
  {
    buttonState = digitalRead (buttonPin);
  }
  lastread = millis(); 
}

void loop() {
  

  // read the state of the pushbutton value:
  //buttonState = digitalRead(buttonPin);

  // check if the pushbutton is pressed. If it is, the buttonState is HIGH:
  if (buttonState == HIGH) {
    // turn LED on:
  Serial.print("Reading state: ");
  Serial.println("on");
    buttonState = LOW;
  } else {
    // turn LED off:
    //Serial.println("off");
  }
  //delay(500);
}
