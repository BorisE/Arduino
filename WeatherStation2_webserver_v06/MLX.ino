void ReadMLXvalues(float &amb, float &obj)
{
  amb = mlx.readAmbientTempC();
  obj = mlx.readObjectTempC();
  
  Serial.print("[!AMB:");
  Serial.print(amb);
  Serial.println("]");
  Serial.print("[!OBJ:");
  Serial.print(obj);
  Serial.println("]");
}
