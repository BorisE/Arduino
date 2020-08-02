void getBMEvalues(float &p, float &t, float &h)
{
  bme.readSensor();
  t=bme.getTemperature_C();
  h=bme.getHumidity();
  p=bme.getPressure_mmHg();

  Serial.print("[!BMP:");
  Serial.print(p);
  Serial.println("]");
  Serial.print("[!BMH:");
  Serial.print(h);
  Serial.println("]");
  Serial.print("[!BMT:");
  Serial.print(t);
  Serial.println("]");

}
