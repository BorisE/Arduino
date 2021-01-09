/*
 * How to add new parameter 
 * 1. Add Config structure element:                             <-- config.parameter
 * 2. Make empty WiFiManagerParameter custom_parameter object
 * 3. Make constant with default value
 * 4. Add to LoadDefaults():                                    <-- config.parameter = strlcpy / = DEFAULT_VALUE
 * 4. Add to LoadConfigData():                                  <-- json["..."] | DEFAULT_VALUE;
 * 5. Add to SaveParameters():                                  <-- strcpy()/atoi() from custom_parameter & json["custom_parameter"] = config.parameter
 * 6. Add to PrintConfig()
 * 7. recalculate capacity fo JSON object
 * 8. Add to WiFi_init:                                         <-- custom_parameter.init & wm->addParameter()
 */


void LoadDefaults()
{
  Serial.println(F("[CONFIG] Loading default values..."));
  
  strlcpy (config.POST_URL, DEFAULT_POST_URL, sizeof(config.POST_URL));
  
  config.H1_1_PIN=H1_1_PIN_DEFAULT;
  config.H1_2_PIN=H1_2_PIN_DEFAULT;
  config.H2_1_PIN=H2_1_PIN_DEFAULT;
  config.H2_2_PIN=H2_2_PIN_DEFAULT;

  config.WS1_PIN=WS1_PIN_DEFAULT;
  config.WS2_PIN=WS2_PIN_DEFAULT;
  config.WS3_PIN=WS3_PIN_DEFAULT;

  config.WATERFLOW_PIN=WATERFLOW_PIN_DEFAULT;

  PrintConfig();
}


void PrintConfig()
{
  Serial.println(F("[CONFIG] Current config: "));

  Serial.print(F("[CONFIG]    POST_URL: "));
  Serial.println( configData.POST_URL );

  Serial.print(F("[CONFIG]    H1_1_PIN: "));
  Serial.println( config.H1_1_PIN );
  Serial.print(F("[CONFIG]    H1_2_PIN: "));
  Serial.println( config.H1_2_PIN );
  Serial.print(F("[CONFIG]    H2_1_PIN: "));
  Serial.println( config.H2_1_PIN );
  Serial.print(F("[CONFIG]    H2_2_PIN: "));
  Serial.println( config.H2_2_PIN );

  Serial.print(F("[CONFIG]    WS1_PIN: "));
  Serial.println( config.WS1_PIN );
  Serial.print(F("[CONFIG]    WS2_PIN: "));
  Serial.println( config.WS2_PIN );
  Serial.print(F("[CONFIG]    WS3_PIN: "));
  Serial.println( config.WS3_PIN );

  Serial.print(F("[CONFIG]    WATERFLOW_PIN: "));
  Serial.println( config.WATERFLOW_PIN );

  Serial.println(F("[CONFIG] end of current config"));
 
}
