#define TRIGGER_PIN D4
#define ECHO_PIN D3

#define USONIC_DIV 58.0

#define MEASURE_SAMPLE_DELAY 5
#define MEASURE_SAMPLES 20
#define MEASURE_DELAY 25

void setup()
{

  Serial.begin(115200); // Serial monitoring
  
  pinMode(TRIGGER_PIN, OUTPUT); // Initializing Trigger Output and Echo Input
  pinMode(ECHO_PIN, INPUT);
  digitalWrite(TRIGGER_PIN, LOW); // Reset the trigger pin and wait a half a second
  delayMicroseconds(500);
}

void loop()
{

  delay(MEASURE_DELAY);
  
  long distance = measure();
  
  Serial.print("Distance: ");
  Serial.print(distance + 22);
  Serial.println(" mm");

}

long measure()
{
  long measureSum = 0;
  
  for (int i = 0; i < MEASURE_SAMPLES; i++)
  {
    delay(MEASURE_SAMPLE_DELAY);
    measureSum += singleMeasurement();
  }

  return measureSum / MEASURE_SAMPLES;
}

long singleMeasurement()
{
  long duration = 0; // Measure: Put up Trigger...
  digitalWrite(TRIGGER_PIN, HIGH); // Wait for 11 µs ...
  delayMicroseconds(11); // Put the trigger down ...
  digitalWrite(TRIGGER_PIN, LOW); // Wait for the echo ...
  duration = pulseIn(ECHO_PIN, HIGH);
  
  return (long) (((float) duration / USONIC_DIV) * 10.0);
}
