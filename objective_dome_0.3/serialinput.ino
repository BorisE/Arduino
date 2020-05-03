void ProcessSerial()
{
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  //READ FROM SERIAL
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  int bufcnt=0; bool exitflag= false;
  while( Serial.available() && bufcnt < 30 && !exitflag) 
  {  
     bufch = Serial.read();
     //Serial.println(bufch);
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
       Serial.print("command name: [");
       Serial.print(cmdnamebuf);
       Serial.println("]");     
       Serial.print("command val: [");
       Serial.print(cmdvalbuf);
       Serial.println("]");     
       
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
  if (cmdnamebuf=="#OPEN")
  {
    Serial.println("CMD: open cover");
    startFullCycleMove(stepsPerCycle);
  }  
  else if (cmdnamebuf=="#CLOSE")
  {
    Serial.println("CMD: close cover");
    startFullCycleMove(-stepsPerCycle);
  }  
  else if (cmdnamebuf=="#STOP")
  {
    //Stop moving
    Serial.println("CMD: stop moving");
    stopMoving();
  }  
  
  else if (cmdnamebuf=="#MOV")
  {
    //Move some steps
    Serial.println("CMD: move some steps");
    moveSteps(cmdvalbuf.toInt());
  }  
  else if (cmdnamebuf=="#POS")
  {
    //Set current position
    Serial.println("CMD: set current position");
    setPosition(cmdvalbuf.toInt());
  }  
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  //Settings
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

  else if (cmdnamebuf=="#SPD")
  {
    //Set max speed
    setStepperSpeed(cmdvalbuf.toFloat());
  }  
  else if (cmdnamebuf=="#ACL")
  {
    //Set max speed
    setStepperAcceleration(cmdvalbuf.toFloat());
  }    
  else if (cmdnamebuf=="#MND")
  {
    //Set minimum moving distance
    setStepperMinDistance(cmdvalbuf.toInt());
  }
  else if (cmdnamebuf=="#CLN")
  {
    //Set full cycle lenght
    setStepperLoopSteps(cmdvalbuf.toInt());
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



void printCurSettings()
{
  Serial.print ("[!MND:");
  Serial.print (minDistanceBeforeChecking);
  Serial.println("]");
}
