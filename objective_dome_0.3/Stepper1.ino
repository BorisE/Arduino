/* Start Cycle Move */
void startFullCycleMove(int cycleLen)
{
  stepperInitPosition = stepper.currentPosition();
  stepper.move(cycleLen);
  Serial.println("Starting moving full cycle");

#ifdef DEBUG_FLAG
  Serial.print("Full cycle lenght=");
  Serial.println(cycleLen);
  Serial.print("From current pos=");
  Serial.println(stepperInitPosition);
#endif
}

/* Stop moving */
void stopMoving()
{
 
#ifdef DEBUG_FLAG
  Serial.print("Distancle left=");
  Serial.println(stepper.distanceToGo());
  Serial.print("Current pos=");
  Serial.println(stepper.currentPosition());
#endif

  stepper.stop();
  Serial.println("Stoping...");
}

/* Move some steps ahead */
void moveSteps(long moveSteps)
{
  stepperInitPosition = stepper.currentPosition();
  stepper.move(moveSteps);
  Serial.print("Starting moving ");
  Serial.print(moveSteps);
  Serial.println(" steps");

#ifdef DEBUG_FLAG
  Serial.print("From current pos=");
  Serial.println(stepperInitPosition);
#endif
}

/* Set current position*/
void setPosition(long curPos)
{
  stepper.setCurrentPosition(curPos);
  Serial.print("Set current position to ");
  Serial.print(curPos);
  Serial.println(" steps");
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
// Settings
///////////////////////////////////////////////////////////////////////////////////////////////////////////
/* Set moving speed, in RPM */
void setStepperSpeed(float SetMotorSpeed)
{
  motorMaxSpeed = SetMotorSpeed;
  
  stepper.setMaxSpeed(motorMaxSpeed);
  Serial.print("Setting max speed to: ");
  Serial.print(motorMaxSpeed);
  Serial.println(" sps");
}

/* Set moving speed, in RPM */
void setStepperAcceleration(float SetMotorAcc)
{
  motorAcceleration = SetMotorAcc;
  
  stepper.setAcceleration(motorAcceleration);
  Serial.print("Setting acceleration to: ");
  Serial.print(motorAcceleration);
  Serial.println(" sps^2");
}

/* Set moving speed, in RPM */
void setStepperMinDistance(int minDistance)
{
  minDistanceBeforeChecking = minDistance;
  
  Serial.print("Setting min distance before checking to: ");
  Serial.print(minDistanceBeforeChecking);
  Serial.println(" steps");
}

/* Set steps per loop cycle */
void setStepperLoopSteps(long SetStepsPerLoop)
{
  stepsPerCycle = SetStepsPerLoop;
  
  Serial.print("Setting steps per loop cycle to: ");
  Serial.print(stepsPerCycle);
  Serial.println(" steps");
}





/* Button status */
void readButtonStatus()
{
  openedButtonStatus = ( digitalRead(OpenedButtonPin) != OPENEDSTATUS );
  closedButtonStatus = ( digitalRead(ClosedButtonPin) != OPENEDSTATUS );
  


 

}
