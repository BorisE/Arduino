WiFiManagerParameter custom_post_url;
WiFiManagerParameter custom_OneWirePin;

//flag for saving data
bool shouldSaveConfig = false;
bool shouldSaveParameters = false;

void LoadConfigData()
{
  //Load OneWire Bus pin number

  //1. Load from somewhere to ONE_WIRE_BUS_PIN_ST as char*
  itoa(ONE_WIRE_BUS_PIN_DEFAULT, ONE_WIRE_BUS_PIN_ST,10);
  Serial.println(ONE_WIRE_BUS_PIN_ST);
  //2. Convert from char* to int if needed
  ONE_WIRE_BUS_PIN = atoi(ONE_WIRE_BUS_PIN_ST);
  Serial.println(ONE_WIRE_BUS_PIN);
}


void SaveParameters()
{
  //read updated parameters
  strcpy(POST_URL, custom_post_url.getValue());
  strcpy(ONE_WIRE_BUS_PIN_ST, custom_OneWirePin.getValue());
  
  Serial.println("Saving config...");
  Serial.print("POST_URL URL: ");
  Serial.println(POST_URL);
  Serial.print("ONE_WIRE_BUS_PIN: ");
  Serial.println(ONE_WIRE_BUS_PIN_ST);
}
