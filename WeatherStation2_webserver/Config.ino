const int capacity=JSON_OBJECT_SIZE(2)+150; //calc here https://arduinojson.org/v6/assistant/        

WiFiManagerParameter custom_post_url;
WiFiManagerParameter custom_OneWirePin;

//flag for saving data
bool shouldSaveConfig = false;
bool shouldSaveParameters = false;


void LoadDefaults()
{
  Serial.println(F("[CONFIG] Load default values..."));
  strlcpy (config.POST_URL, DEFAULT_POST_URL, sizeof(config.POST_URL));
  config.OneWirePin     = ONE_WIRE_BUS_PIN_DEFAULT;

  PrintConfig();
}


void LoadConfigData()
{
  //read configuration from FS json
  Serial.println(F("[SPIFFS] mounting FS..."));

  if (SPIFFS.begin()) {

    listAllFiles();
    printConfigFile();

    //file exists?
    if (SPIFFS.exists(configFilename)) {
      
      Serial.println(F("[SPIFFS] reading config file..."));
      File configFile = SPIFFS.open(configFilename, "r");
      
      if (configFile) {
        //Deserialize
        StaticJsonDocument<capacity> json;
        
        // Deserialize the JSON document
        DeserializationError error = deserializeJson(json, configFile);
        if (error)
        {
          Serial.println(F("[CONFIG] Failed to read file"));
        }
        else
        {
          Serial.println(F("[CONFIG] successufuly parsed json config file"));

          // print contents
          Serial.print(F("[CONFIG] loaded JSON: "));
          serializeJson(json, Serial);
          Serial.println();        

          // Copy values from the JsonDocument to the Config
          config.OneWirePin = json["OneWirePin"] | ONE_WIRE_BUS_PIN_DEFAULT;
          strlcpy (config.POST_URL, json["POST_URL"] | DEFAULT_POST_URL, sizeof(config.POST_URL));         // <- destination // <- source // <- destination's capacity

          // Close the file (Curiously, File's destructor doesn't close the file)
          configFile.close();
          
        }
      } else {
        Serial.println(F("[SPIFFS] couldn't open config file"));
      }
    } else {
      Serial.println(F("[SPIFFS] config file not found"));
    }
  } else {
    Serial.println("[SPIFFS] failed to mount FS");
  }
  //end read
  PrintConfig();
 
//  
//  //Load OneWire Bus pin number
//
//  //1. Load from somewhere to ONE_WIRE_BUS_PIN_ST as char*
//  itoa(ONE_WIRE_BUS_PIN_DEFAULT, ONE_WIRE_BUS_PIN_ST,10);
//  Serial.println(ONE_WIRE_BUS_PIN_ST);
//  //2. Convert from char* to int if needed
//  ONE_WIRE_BUS_PIN = atoi(ONE_WIRE_BUS_PIN_ST);
//  Serial.println(ONE_WIRE_BUS_PIN);
}


void SaveParameters()
{
  PrintConfig();
  
  //read updated parameters
  strcpy(config.POST_URL, custom_post_url.getValue());
  config.OneWirePin = atoi(custom_OneWirePin.getValue());

  StaticJsonDocument<capacity> json;

  json["POST_URL"]              = config.POST_URL;
  json["OneWirePin"]            = config.OneWirePin;

  //Print config object
  Serial.print(F("[CONFIG] saving JSON: "));
  serializeJson(json, Serial);
  Serial.println();

  // Write to file
  if (SPIFFS.begin()) {
    //file exists?
    if (SPIFFS.exists(configFilename)) {
        if (!SPIFFS.remove(configFilename))
        {
          Serial.println(F("[SPIFFS] can't delete old config file"));
        } else {
          Serial.println(F("[SPIFFS] old config file was deleted"));
        }
    }
    // Open file for writing
    File file = SPIFFS.open(configFilename, "w");
    if (!file) {
      Serial.println(F("[SPIFFS] Failed to create file"));
      return;
    }    
    // Serialize JSON to file
    if (serializeJson(json, file) == 0) {
      Serial.println(F("[CONFIG] Failed to serialize to file"));
    } else {
      Serial.println(F("[CONFIG] Configuration saved"));
    }
    file.close();
  } else {
    Serial.println(F("[SPIFFS] failed to mount FS"));
  }

  PrintConfig();
  
  listAllFiles();
}

void PrintConfig()
{
  Serial.print("[CONFIG] POST_URL: ");
  Serial.println( config.POST_URL );

  Serial.print("[CONFIG] OneWirePin: ");
  Serial.println( config.OneWirePin );
}

void listAllFiles()
{
  SPIFFS.begin();
  Serial.println(F("[SPIFFS] listing files:"));
  
  Dir dir = SPIFFS.openDir ("");
  while (dir.next ()) {
    Serial.print (dir.fileName ());
    Serial.print ("  | ");
    Serial.println (dir.fileSize ());
  }
  FSInfo fs_info;
  SPIFFS.info(fs_info);

  Serial.print("fs_info.totalBytes = ");
  Serial.println(fs_info.totalBytes);
  Serial.print("fs_info.usedBytes = ");
  Serial.println(fs_info.usedBytes);
  Serial.print("fs_info.blockSize = ");
  Serial.println(fs_info.blockSize);
  Serial.print("fs_info.pageSize = ");
  Serial.println(fs_info.pageSize);
  Serial.print("fs_info.maxOpenFiles = ");
  Serial.println(fs_info.maxOpenFiles);
  Serial.print("fs_info.maxPathLength = ");
  Serial.println(fs_info.maxPathLength);  
}


void printConfigFile() {
  Serial.println(F("[CONFIG] print config file contents:"));
  File f = SPIFFS.open(configFilename, "r");
  if (!f)
    Serial.println(F("config file open failed"));
  else {
    Serial.print("<<<");
    while (f.available())
      Serial.write(f.read());
    f.close();
  }
  Serial.println(">>>");
}
