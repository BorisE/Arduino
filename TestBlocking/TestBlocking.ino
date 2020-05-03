// Blocking.pde
// -*- mode: C++ -*-
//
// Shows how to use the blocking call runToNewPosition
// Which sets a new target position and then waits until the stepper has 
// achieved it.
//
// Copyright (C) 2009 Mike McCauley
// $Id: Blocking.pde,v 1.1 2011/01/05 01:51:01 mikem Exp mikem $

#include <AccelStepper.h>
#define HALFSTEP 8

// Define a stepper and the pins it will use
//AccelStepper stepper; // Defaults to AccelStepper::FULL4WIRE (4 pins) on 2, 3, 4, 5
AccelStepper stepper(AccelStepper::HALF4WIRE, 26, 28, 30, 32);

void setup()
{  
    Serial.begin(9600);
    
    stepper.setMaxSpeed(100.0);
    stepper.setAcceleration(50.0);
    stepper.setSpeed(100);
    
}

void loop()
{    
    Serial.println(stepper.currentPosition ());
    stepper.runToNewPosition(0);
    Serial.println(stepper.currentPosition ());
    stepper.runToNewPosition(2000);
}
