
char *ss_str[] = {
  "WS1 become 1, waiting to start close procedure", // 0
  "WS1 become 0, waiting to start open procedure",  // 1
  "Delay finished, start VENT1 close procedure",    // 2
  "Delay finished, start VENT1 open procedure",     // 3
  "VENT1 changing state timeout reached, switching off",     // 4
};


/****************************************************
*
* Procedure for opening/closing VENT1 on WaterSensor change state
*   WS1 from 0 to 1 - close (with timeout)
*   WS1 from 1 to 0 - open  (with timeout)
*
****************************************************/
void checkSupplyStatus_changeState() {

  //1. Get WS TOP state
  WS_top_flag = digitalRead(config.WS1_PIN);
  //    char str[100] = "WS1=";
  //    itoa(, str + strlen(str), DEC);
  //    debug(str);

  //2.  If WS TOP state changed from 0 to 1
  //    init close sequence but in a defined timeout
  if (WS_top_flag==1 && lastWS_top_flag ==0 ) {
      debug(ss_str[0]);
      
      needToClose_flag = 1;
      needToOpen_flag = 0;
      waitToEngageVent1_starttime = currenttime;
  }

  //3.  if WS TOP state changed from 1 to 0
  //    init open sequence 
  if (WS_top_flag==1 && lastWS_top_flag == 0 ) {
      debug(ss_str[1]);

      needToOpen_flag=1;
      needToClose_flag = 0;
      waitToEngageVent1_starttime = currenttime;
  }

  //4.  Check if it is time to close vent
  if (needToClose_flag == 1 && (currenttime - waitToEngageVent1_starttime) > VENT_CLOSE_DELAY) {
      debug(ss_str[2]);

      relaySwitchByParams(VENT1_CLOSE_RELAY_NAME, "1");
      relaySwitchByParams(VENT1_OPEN_RELAY_NAME,  "0");
      needToClose_flag=0;

      Vent1_ChangingState = 1;
      Vent1_ChangeState_starttime = currenttime;
  }

  //5.  Check if it is time to open vent
  if (needToOpen_flag == 1 && (currenttime - waitToEngageVent1_starttime) > VENT_CLOSE_DELAY) {
      debug(ss_str[3]);

      relaySwitchByParams(VENT1_OPEN_RELAY_NAME,  "1");
      relaySwitchByParams(VENT1_CLOSE_RELAY_NAME, "0");
      needToOpen_flag=0;

      Vent1_ChangingState = 1;
      Vent1_ChangeState_starttime = currenttime;
  }

  //6.  Check if it is time to stop change state procedure
  if (Vent1_ChangingState == 1 && (currenttime - Vent1_ChangeState_starttime) > VENT_CHANGESTATE_TIMEOUT ) {
      debug(ss_str[4]);

      relaySwitchByParams(String(VENT1_CLOSE_RELAY_NAME), "0");
      relaySwitchByParams(String(VENT1_OPEN_RELAY_NAME),  "0");
      Vent1_ChangingState = 0;
  }

  //Save current WS_top state
  lastWS_top_flag = WS_top_flag;
 
}
