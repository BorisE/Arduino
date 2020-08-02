WiFiManager wm;

/*
 * init WiFiManager data
 */
void WiFi_init()
{
  // custom menu via array or vector
  // 
  // menu tokens, "wifi","wifinoscan","info","param","close","sep","erase","restart","exit" (sep is seperator) (if param is in menu, params will not show up in wifi page!)
  // const char* menu[] = {"wifi","info","param","sep","restart","exit"}; 
  // wm.setMenu(menu,6);
  std::vector<const char *> menu = {"wifi","param","sep","info","sep","restart","exit"};
  wm.setMenu(menu);

}

/*
 *  WiFi Manager COnnection Procedure
 */
void WiFi_CheckConnection()
{
  // Wait for connection
  if (WiFi.status() != WL_CONNECTED) 
  {
    if (WiFi.SSID().length() != 0)
    {
      Serial.println("There is saved AP credentials. So will give them a try after timeout");
      Serial.print("SSID: ");
      Serial.println(WiFi.SSID());
      Serial.print("PSK: ");
      if (WiFi.psk().length() != 0)
        Serial.println("********");
      
      // if there was saved credentials
      // then there is can be no connection to WiFi at controller start
      // so it is wise to try again in some time interval
      wm.setConfigPortalTimeout( WIFI_CONFIG_PORTAL_WAITTIME ); // auto close configportal after n seconds
    }

    digitalWrite(STATUS_LED, HIGH);
    Serial.print(".");
    delay(400);
    digitalWrite(STATUS_LED, LOW);
    delay(100);

    bool res;
    res = wm.autoConnect(ssid); // Start 
    if(!res) {
        Serial.println("Failed to connect");
    } 

    if (WiFi.status() == WL_CONNECTED) 
    {
      Serial.println("");
      Serial.print("Connected to ");
      Serial.println(WiFi.SSID());
      Serial.print("IP address: ");
      Serial.println(WiFi.localIP());
    }
  }
}

void startConfigPortal()
{
  if (!wm.startConfigPortal(ssid)) {
    Serial.println("Failed to connect or hit timeout");
    // ESP.restart();
  }

  if (WiFi.status() == WL_CONNECTED) 
  {
    Serial.println("");
    Serial.print("Connected to ");
    Serial.println(WiFi.SSID());
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
  }
}
