void ProcessSerial()
{
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  //READ FROM SERIAL
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  int bufcnt=0; bool exitflag= false;
  while( Serial.available() && bufcnt < 30 && !exitflag) 
  {  
     bufch = Serial.read();
     //Serial.println(ch);
     bufcnt++;
     if (bufch=='('){
       startflag=true;
       cmdnameflag=true;
       cmdvalflag=false;
       cmdbuf="";
       cmdnamebuf="";
       cmdvalbuf="";
     }else if (bufch==')'){
       cmdbuf += bufch;
       startflag=false;
       cmdnameflag=false;
       cmdvalflag=false;
       exitflag=true;
       Serial.print("Input command: [");
       Serial.print(cmdbuf);
       Serial.println("]");
/*       Serial.print("command name: [");
       Serial.print(cmdnamebuf);
       Serial.println("]");     
       Serial.print("command val: [");
       Serial.print(cmdvalbuf);
       Serial.println("]");     
*/       
     } else if (bufch==':'){
       cmdnameflag=false;
       cmdvalflag=true;
       cmdvalbuf="";
     }
     if (startflag==true){
       cmdbuf += bufch;
     }
     if (cmdnameflag==true && bufch!='('){
       cmdnamebuf += bufch;
     }
     if (cmdvalflag==true && bufch!=':'){
       cmdvalbuf += bufch;
     }
  }

  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  //Process commands from serial
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  if (cmdbuf=="(RL1:0)")
  {
    //Switch relay off
    Serial.println("Switching relay OFF...");
    RelayHeatingOFF();
  }  
  else if (cmdbuf=="(RL1:1)")
  {
    //Switch relay on
    Serial.println("Switching relay ON...");
    RelayHeatingON();
  }
  else if (cmdnamebuf=="!TD")
  {
    //Set heating max temp delta
    TEMP_DELTA_LIMIT=cmdvalbuf.toInt();
    Serial.print("Temp delta: ");
    Serial.println(TEMP_DELTA_LIMIT);
  }
  else if (cmdnamebuf=="!WT")
  {
    //Set wet threshold limit
    WET_THRESHOLD=cmdvalbuf.toInt();
    Serial.print("Wet threshold: ");
    //Serial.print(cmdvalbuf);
    //Serial.print(" ");
    Serial.println(WET_THRESHOLD);
  }
  else if (cmdnamebuf=="!RT")
  {
    //Set max heating runtime cycle
    RELAY_RUN_TIME_LIMIT=cmdvalbuf.toInt()*1000;
    Serial.print("Runtime: ");
    Serial.println(RELAY_RUN_TIME_LIMIT);
  }
  else if (cmdnamebuf=="!?S")
  {
    //Print current settings
    printCurSettings();
  }
  else
  {
    //Serial.print("unknown command [");
    //Serial.print(cmdbuf);
    //Serial.println("]");
  }

  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  //Empty command buffer
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  cmdbuf="";
  cmdnamebuf="";
  cmdvalbuf="";
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void RelayHeatingON()
{
    digitalWrite(RELAY_1_PIN,RELAY_ON);
    relaystart = millis();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void RelayHeatingOFF()
{
    digitalWrite(RELAY_1_PIN,RELAY_OFF);
    relaystart = 0;
}

void printCurSettings()
{
    Serial.print("[!?TD:");
    Serial.print(TEMP_DELTA_LIMIT);
    Serial.println("]");
    Serial.print("[!?WT:");
    Serial.print(WET_THRESHOLD);
    Serial.println("]");
    Serial.print("[!?RT:");
    Serial.print(RELAY_RUN_TIME_LIMIT);
    Serial.println("]");
  
}
