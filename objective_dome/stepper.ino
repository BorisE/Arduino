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

/* Set moving speed, in RPM */
void setMoveStepperSpeed(long motorSpeed)
{
  myStepper.setSpeed(motorSpeed);
  Serial.print("Setting speed to: ");
  Serial.print(motorSpeed);
  Serial.println(" rpm");
}

/* Make one stepper move */
void stepperMove()
{
  myStepper.step(stepsPerRevolution * currentDirection);
  currentStepPos += stepsPerRevolution * currentDirection;
  
#ifdef DEBUG_FLAG
  Serial.print("Stepper Direction, sign: ");
  Serial.println(currentDirection);
  Serial.print("Move on: ");
  Serial.println(stepsPerRevolution * currentDirection);
#endif
}
