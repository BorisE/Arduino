
void calcWaterFlow() {
  //Serial.println(flow_count);           // Print litres/hour
  
  unsigned long passedMillSec = (currenttime-_lastReadTime_WaterFlow);
  if (_lastReadTime_WaterFlow == 0) passedMillSec = WATERFLOW_COUNT_FREQUENCY;
  
  // Pulse frequency (Hz) = (6.6*Q)  , Q=L/MIN±3%
  flow_l_min = (flow_count / (passedMillSec / 1000.0) / 6.6);      // (Pulse frequency x 60 min) / 6.6Q = flowrate in L/hour

  // Print litres/hour
  Serial.print("[Flow: ");            
  Serial.print(flow_l_min, DEC);            
  Serial.println(" L/min]");

  flow_count = 0;                       // Reset Counter
}
