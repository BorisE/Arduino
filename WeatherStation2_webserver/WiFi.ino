
//callback notifying us of the need to save config
void saveConfigCallback () {
  Serial.println(F("Should save config"));
  shouldSaveConfig = true;
}

/*
 * init WiFiManager data
 */
void WiFi_init(WiFiManager* wm, int WaitTime = 0 )
{
  wm->setDebugOutput(true);
  wm->setShowPassword(false);
  
  // custom menu via array or vector
  // 
  // menu tokens, "wifi","wifinoscan","info","param","close","sep","erase","restart","exit" (sep is seperator) (if param is in menu, params will not show up in wifi page!)
  // const char* menu[] = {"wifi","info","param","sep","restart","exit"}; 
  // wm.setMenu(menu,6);
  std::vector<const char *> menu = {"wifi","sep","info","sep","restart","exit"};
  wm->setMenu(menu);

  wm->setSaveConfigCallback(saveConfigCallback);


  // if there was saved credentials
  // then there is can be no connection to WiFi at controller start
  // so it is wise to try again in some time interval
  //if (WiFi.SSID().length() != 0) -make this permanent
  {
    if (WaitTime ==0) WaitTime = WIFI_CONFIG_PORTAL_WAITTIME;
    wm->setConfigPortalTimeout( WaitTime ); // auto close configportal after n seconds
  }

  //blink led
  digitalWrite(STATUS_LED, HIGH);
  Serial.print(".");
  delay(400);
  digitalWrite(STATUS_LED, LOW);
  delay(200);
  digitalWrite(STATUS_LED, HIGH);
}


/*
 *  WiFi Manager Connection Procedure
 *  first try autoconnect to saved wifi credentials
 */
void WiFi_CheckConnection(int WaitTime)
{
  // Wait for connection
  if (WiFi.status() != WL_CONNECTED) 
  {
    WiFiManager wm;
    WiFi_init(&wm, WaitTime);
    WiFiManagerParameter custom_post_url("post_url_id", "POST URL", POST_URL, 40);
    WiFiManagerParameter custom_OneWirePin("OneWirePinId", "OneWire pin", ONE_WIRE_BUS_PIN_ST, 2);
    wm.addParameter(&custom_post_url);
    wm.addParameter(&custom_OneWirePin);
    
//    if (WiFi.SSID().length() != 0)
//    {
//      Serial.println(F("There is saved AP credentials. So will give them a try after timeout"));
//      Serial.print("SSID: ");
//      Serial.println(WiFi.SSID());
//      Serial.print("PSK: ");
//      if (WiFi.psk().length() != 0)
//        Serial.println("********");
//    }

    bool res = wm.autoConnect(ssid); // Start 
    if(!res) {
        Serial.println(F("Failed to connect or hit timeout"));
    } 

    //save parameters before wm object unloaded
    if (shouldSaveConfig) {
      //read updated parameters
      strcpy(POST_URL, custom_post_url.getValue());
      strcpy(ONE_WIRE_BUS_PIN_ST, custom_OneWirePin.getValue());

      SaveParameters();
      
      //end save
      shouldSaveConfig = false;
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

/*
 * Call ConfigPortal and set ESP to AP mode
 * without even connection attempt
 */
void startConfigPortal()
{
  WiFiManager wm;
  WiFi_init(&wm);
//  WiFiManagerParameter custom_post_url("post_url_id", "POST URL", POST_URL, 40);
//  WiFiManagerParameter custom_OneWirePin("OneWirePinId", "OneWire pin", ONE_WIRE_BUS_PIN_ST, 2);
//  wm.addParameter(&custom_post_url);
//  wm.addParameter(&custom_OneWirePin);

  if (!wm.startConfigPortal(ssid)) {
    Serial.println(F("Failed to connect or hit timeout"));
    // ESP.restart();
  }
  
  //save parameters before wm object unloaded
  if (shouldSaveConfig) {
    //read updated parameters
//    strcpy(POST_URL, custom_post_url.getValue());
//    strcpy(ONE_WIRE_BUS_PIN_ST, custom_OneWirePin.getValue());
    
//    SaveParameters();

    //end save
    shouldSaveConfig = false;
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
