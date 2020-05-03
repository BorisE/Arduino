/* get current direction */
int dummyGetDirection()
{
  if (openedButtonStatus == 1 && closedButtonStatus == 0)
  {
    currentDirection = directionClose;
  }
  else if (openedButtonStatus == 0 && closedButtonStatus == 1)
  {
    currentDirection = directionOpen;
  }

  return currentDirection;
}

/* reverse direction */
void reverseDirection()
{
  currentDirection = ( currentDirection == directionOpen? directionClose: directionOpen);
}

void setDirection(int newDir)
{
  currentDirection = ( newDir == directionOpen ? directionOpen : directionClose);
}


/* Set moving speed, in RPM */
void setMoveStepperSpeed(long SetMotorSpeed)
{
  motorSpeed = SetMotorSpeed;
  
  myStepper.setSpeed(motorSpeed);
  Serial.print("Setting speed to: ");
  Serial.print(motorSpeed);
  Serial.println(" rpm");
}


/* Set steps per loop cycle */
void setStepperLoopSteps(long SetStepsPerLoop)
{
  stepsPerLoopCycle = SetStepsPerLoop;
  
  Serial.print("Setting steps per loop cycle to: ");
  Serial.print(stepsPerLoopCycle);
  Serial.println(" steps");
}


/* Make one stepper move */
void stepperMove()
{
  myStepper.step(stepsPerLoopCycle * currentDirection);
  currentStepPos = currentStepPos + stepsPerLoopCycle * currentDirection;
  
#ifdef DEBUG_FLAG
  Serial.print("Stepper Direction, sign: ");
  Serial.println(currentDirection);
  Serial.print("Move on: ");
  Serial.println(stepsPerLoopCycle * currentDirection);
#endif
}


void readButtonStatus()
{
  openedButtonStatus = ( digitalRead(OpenedButtonPin) != OPENEDSTATUS );
  closedButtonStatus = ( digitalRead(ClosedButtonPin) != OPENEDSTATUS );
  
  Serial.print("[!OPN:");
  Serial.print(openedButtonStatus, 1);
  Serial.println("]");
  Serial.print("[!CLS:");
  Serial.print(closedButtonStatus, 1);
  Serial.println("]");

 
#ifdef DEBUG_FLAG
  if (openedButtonStatus==1 && closedButtonStatus==0)
  {
    Serial.println("Cover is opened");
  }
  else if (openedButtonStatus==0 && closedButtonStatus==1)
  {
    Serial.println("Cover is closed");
  }
  else if (openedButtonStatus==0 && closedButtonStatus==0)
  {
    Serial.println("Cover is in mid position");
  }
  else
  {
    Serial.println("Cover position error!");
  }
#endif
}
