void switchOn()
{
  digitalWrite(RELAY_PUMP_PIN, LOW);
  Serial.println("Switching PUMP ON"); 

}
void switchOff()
{
  digitalWrite(RELAY_PUMP_PIN, HIGH);
  Serial.println("Switching PUMP OFF"); 
}
