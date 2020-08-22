void onStartOTA() {
  String type;  
  if (ArduinoOTA.getCommand() == U_FLASH)  
  {  
    type = "sketch";  
  }   
  else  
  { // U_SPIFFS  
    type = "filesystem";  
  }  
  // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()  
  Serial.print(F("[OTA] Start updating "));
  Serial.println(type);  
}


void onErrorOTA(ota_error_t error) {
  Serial.printf("[OTA] Error[%u]: ", error);
  if (error == OTA_AUTH_ERROR)          Serial.println(F("Auth Failed"));
  else if (error == OTA_BEGIN_ERROR)    Serial.println(F("Begin Failed"));
  else if (error == OTA_CONNECT_ERROR)  Serial.println(F("Connect Failed"));
  else if (error == OTA_RECEIVE_ERROR)  Serial.println(F("Receive Failed"));
  else if (error == OTA_END_ERROR)      Serial.println(F("End Failed"));
}
