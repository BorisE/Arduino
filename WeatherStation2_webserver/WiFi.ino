
//callback notifying us of the need to save config
void saveConfigWiFICallback () {
  Serial.println(F("Should save WiFi config"));
  shouldSaveConfig = true;
}

//callback notifying us of the need to save config
void saveParamsCallback () {
  Serial.println(F("Should save parameters"));
  shouldSaveParameters = true;
}

/*
 * init WiFiManager data
 */
void WiFi_init(WiFiManager* wm, int WaitTime = 0 )
{
  digitalWrite(STATUS_LED, HIGH);

  wm->setDebugOutput(true);
  wm->setShowPassword(false);

  //void init(const char *id, const char *label, const char *defaultValue, int length, const char *custom, int labelPlacement);
  //WiFiManagerParameter custom_post_url("post_url_id", "POST URL", POST_URL, 40);
  //WiFiManagerParameter custom_OneWirePin("OneWirePinId", "OneWire pin", ONE_WIRE_BUS_PIN_ST, 2);
  custom_post_url.init  ("post_url_id",   "POST URL",       configData.POST_URL,  101, "", WFM_LABEL_BEFORE);
  custom_OneWirePin.init("OneWirePinId",  "OneWire pin",    configData.OneWirePin,  2, "", WFM_LABEL_BEFORE);
  custom_I2CSDAPin.init ("I2CSDAPinId",   "I2C SDA pin",    configData.I2CSDAPin,   2, "", WFM_LABEL_BEFORE);
  custom_I2CSCLPin.init ("I2CSCLPinId",   "I2C SCL pin",    configData.I2CSCLPin,   2, "", WFM_LABEL_BEFORE);
  custom_DHT22Pin.init  ("DHT22PinId",    "DHT22 pin",      configData.DHT22Pin,    2, "", WFM_LABEL_BEFORE);
  custom_RainPin.init   ("RainPinId",     "RainSensor pin", configData.RainPin,     2, "", WFM_LABEL_BEFORE);

  wm->addParameter(&custom_post_url);
  wm->addParameter(&custom_OneWirePin);
  wm->addParameter(&custom_I2CSDAPin);
  wm->addParameter(&custom_I2CSCLPin);
  wm->addParameter(&custom_DHT22Pin);
  wm->addParameter(&custom_RainPin);
  
  // custom menu via array or vector
  // 
  // menu tokens, "wifi","wifinoscan","info","param","close","sep","erase","restart","exit" (sep is seperator) (if param is in menu, params will not show up in wifi page!)
  // const char* menu[] = {"wifi","info","param","sep","restart","exit"}; 
  // wm.setMenu(menu,6);
  std::vector<const char *> menu = {"wifi","param","sep","info","sep","restart","exit"};
  wm->setMenu(menu);

  wm->setSaveConfigCallback(saveConfigWiFICallback); //when wifi credentials are saved
  wm->setSaveParamsCallback(saveParamsCallback); //when parameters are saved


  // if there was saved credentials
  // then there is can be no connection to WiFi at controller start
  // so it is wise to try again in some time interval
  //if (WiFi.SSID().length() != 0) -make this permanent
  {
    if (WaitTime ==0) WaitTime = WIFI_CONFIG_PORTAL_WAITTIME;
    wm->setConfigPortalTimeout( WaitTime ); // auto close configportal after n seconds
  }

  wm->setAPCallback(configModeCallback);

//  //blink led
//  digitalWrite(STATUS_LED, HIGH);
//  Serial.print(".");
//  delay(400);
//  digitalWrite(STATUS_LED, LOW);
//  delay(200);
//  digitalWrite(STATUS_LED, HIGH);
}

/*
 * Finilize WiFi manager data
 */
void WiFi_deinit()
{
  // stop blinking
  ticker.detach();
  //keep LED on
  digitalWrite(STATUS_LED, LOW);

  //save parameters before wm object unloaded
  if (shouldSaveConfig || shouldSaveParameters) {
    SaveParameters();
    //end save
    shouldSaveConfig = false;
    shouldSaveParameters = false;
    
    //reboot to account hardware changes
    ESP.restart();
  }

  if (WiFi.status() == WL_CONNECTED) 
  {
    Serial.println("");
    Serial.print(F("Connected to "));
    Serial.println(WiFi.SSID());
    Serial.print(F("IP address: "));
    Serial.println(WiFi.localIP());
  }
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
    
    WiFi_deinit();
  }
}

/*
 * Call ConfigPortal and set ESP to AP mode
 * without even connection attempt
 */
void runConfigPortal()
{
  WiFiManager wm;
  WiFi_init(&wm);

  if (!wm.startConfigPortal(ssid)) {
    Serial.println(F("Failed to connect or hit timeout"));
    // ESP.restart();
  }
  
  WiFi_deinit();
}

//for led blinking 
void tick()
{
  //toggle state
  digitalWrite(STATUS_LED, !digitalRead(STATUS_LED));     // set pin to the opposite state
}

//gets called when WiFiManager enters configuration mode
void configModeCallback (WiFiManager *myWiFiManager) {
  Serial.println(F("Entered config mode"));
  Serial.println(WiFi.softAPIP());
  //if you used auto generated SSID, print it
  Serial.println(myWiFiManager->getConfigPortalSSID());
  //entered config mode, make led toggle faster
  ticker.attach(0.2, tick);
}
