void initRelays()
{
  pinMode(config.H1_1_PIN, OUTPUT);    // Установим вывод 5 как выход
  pinMode(config.H1_2_PIN, OUTPUT);    // Установим вывод 6 как выход
  pinMode(config.H2_1_PIN, OUTPUT);    // Установим вывод 6 как выход
  pinMode(config.H2_2_PIN, OUTPUT);    // Установим вывод 6 как выход

  digitalWrite(config.H1_1_PIN, HIGH); // Выключаем реле
  digitalWrite(config.H1_2_PIN, HIGH); // Выключаем реле
  digitalWrite(config.H2_1_PIN, HIGH); // Выключаем реле
  digitalWrite(config.H2_2_PIN, HIGH); // Выключаем реле

}

/*********************************************************************
// Switch Relay
//    relayName - "relay1", "relay2", etc
//    relayTargetStat - target relay state "1"|"0" ("on"|"off")
**********************************************************************/
int relaySwitchByParams(String relayName, String relayTargetStat ) {
  const char *ss[] = {
    "Relay ",
    " is set to ",
    };
  char st[100]={'\0'};

  int pin = getRelayPinByName (relayName);
  if (pin >= 0) {
    int targetStat = convertReyalyStatusToInt(relayTargetStat);
    if ( targetStat >=0 ) {
      digitalWrite(pin, targetStat);
      strcat(st,ss[0]);
      strcat(st,relayName.c_str());
      strcat(st,ss[1]);
      strcat(st,relayTargetStat.c_str());
      debug(st);
      return 1;
    } else {
      return -2;
    }
  }else {
    return -1;
  }
}

String getRelayStatusString(int relayPin) {
  uint8_t res = digitalRead(relayPin);
  if (res == LOW)
    return "ON";
  else
    return "OFF";
}


int getRelayPinByName(String relay) {

  if (relay=="relay1")
     return config.H1_1_PIN;
  else if (relay=="relay2")
    return config.H1_2_PIN;
  else if (relay=="relay3")
    return config.H2_1_PIN;
  else if (relay=="relay4")
    return config.H2_2_PIN;
  else
      return -1;
}


int convertReyalyStatusToInt(String stat) {

  if (stat=="1") 
    return LOW;
  else if (stat=="0") 
    return HIGH;
  else 
    return -1;
}


void printRelayStatus() {
  Serial.print("[H1_1: ");
  Serial.print(digitalRead(config.H1_1_PIN));
  Serial.println("]");
  Serial.print("[H1_2: ");
  Serial.print(digitalRead(config.H1_2_PIN));
  Serial.println("]");
  Serial.print("[H2_1: ");
  Serial.print(digitalRead(config.H2_1_PIN));
  Serial.println("]");
  Serial.print("[H2_2: ");
  Serial.print(digitalRead(config.H2_2_PIN));
  Serial.println("]");
}
