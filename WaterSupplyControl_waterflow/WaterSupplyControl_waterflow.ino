#define WATERFLOW_PIN D8

volatile int flow_frequency; // variable to store the “rise ups” from the flowmeter pulses
unsigned int flow_l_min; // Calculated litres/hour
unsigned long currentTime = 0;
unsigned long cloopTime;

//Interrupt function, so that the counting of pulse “rise ups” dont interfere with the rest of the code  (attachInterrupt)
void ICACHE_RAM_ATTR flow_ISR(){   
  flow_frequency++;
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);

  pinMode(WATERFLOW_PIN, INPUT_PULLUP);
  //digitalWrite(WATERFLOW_PIN, HIGH); // Optional Internal Pull-Up

  // Attach an interrupt to the ISR vector
  attachInterrupt(digitalPinToInterrupt(WATERFLOW_PIN), flow_ISR, RISING);
  sei(); // Enable interrupts
  
  currentTime = millis();
  cloopTime = currentTime;

  Serial.println("Starting");
}

void loop() {
   currentTime = millis();
  
   // Every second, calculate and print litres/hour
   if(currentTime >= (cloopTime + 1000))
   {
      cloopTime = currentTime;                // Updates cloopTime
      Serial.println(flow_frequency);              // Print litres/hour
      
      // Pulse frequency (Hz) = (6.6*Q)  Q=L/MIN±3%
      flow_l_min = (flow_frequency / 6.6);  // (Pulse frequency x 60 min) / 6.6Q = flowrate in L/hour
      flow_frequency = 0;                     // Reset Counter
      Serial.print(flow_l_min, DEC);              // Print litres/hour
      Serial.println(" L/min");
   }
}
