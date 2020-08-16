String getSensorStatusString(int WSensPin)
{
  uint8_t res = digitalRead(WSensPin);
  if (res == LOW)
    return "-";
  else
    return "+";
}

String getSensorStatusString(String WSen)
{
  int WSensPin = getSensorPinByName (WSen);
  return getSensorStatusString(WSensPin);
}


int getSensorPinByName(String WSen) {
  if (WSen=="WS1")
     return config.WS1_PIN;
  else if (WSen=="WS2")
    return config.WS2_PIN;
  else if (WSen=="WS3")
    return config.WS3_PIN;
  else
      return -1;
}


void printWSensorStatus() {
  Serial.print("[WS1: ");
  Serial.print(digitalRead(config.WS1_PIN));
  Serial.println("]");
  Serial.print("[WS2: ");
  Serial.print(digitalRead(config.WS2_PIN));
  Serial.println("]");
  Serial.print("[WS3: ");
  Serial.print(digitalRead(config.WS3_PIN));
  Serial.println("]");
}
