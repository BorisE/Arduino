void setup() {
  Serial.begin(9600); //Start Serial Monitor to display current read value on Serial monitor
  
  pinMode(7, OUTPUT);
  digitalWrite(7, LOW);
}


#define AMP_SAMPLING_NUMBER 500
#define AMP_PIN A5
void loop() {
  unsigned int x=0;
  float AcsValue=0.0,Samples=0.0,AvgAcs=0.0,AcsValueF=0.0;
  
    for (int x = 0; x < AMP_SAMPLING_NUMBER; x++){ //Get 150 samples
    AcsValue = analogRead(AMP_PIN);     //Read current sensor values   
    Samples = Samples + AcsValue;  //Add samples together
    delay (3); // let ADC settle before next sample 3ms
  }
  AvgAcs=Samples/float(AMP_SAMPLING_NUMBER);//Taking Average of Samples
  
  //((AvgAcs * (5.0 / 1024.0)) is converitng the read voltage in 0-5 volts
  //2.5 is offset(I assumed that arduino is working on 5v so the viout at no current comes
  //out to be 2.5 which is out offset. If your arduino is working on different voltage than 
  //you must change the offset according to the input voltage)
  //0.185v(185mV) is rise in output voltage when 1A current flows at input
  AcsValueF = (2.5 - (AvgAcs * (5.0 / 1024.0)) )/0.185 + 0.309;
  
  Serial.println(AcsValueF);//Print the read current on Serial monitor
  //delay(50);
}
