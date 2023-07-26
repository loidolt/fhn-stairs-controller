// Bounce.pde
// -*- mode: C++ -*-
//
// Make a single stepper bounce from one limit to another
//
// Copyright (C) 2012 Mike McCauley
// $Id: Random.pde,v 1.1 2011/01/05 01:51:01 mikem Exp mikem $

#include <AccelStepper.h>

const int stepperPulsePin = 5;
const int stepperDirectionPin = 6;
const int stepperEnablePin = 7;
const int stepperAlarmPin = 8;

// Define a stepper and the pins it will use
// not using default 4 wires setup, but instead using step, direction, and enable pins
AccelStepper stepper(AccelStepper::DRIVER, stepperPulsePin, stepperDirectionPin);

void setup() {
  // Change these to suit your stepper if you want
  stepper.setEnablePin(stepperEnablePin);
  stepper.setPinsInverted(false, false, true);
  stepper.enableOutputs();
  stepper.setMaxSpeed(1000);
  stepper.setAcceleration(100);
  stepper.moveTo(8000);
}

void loop() {
  // If at the end of travel go to the other end
  if (stepper.distanceToGo() == 0)
    stepper.moveTo(-stepper.currentPosition());

  stepper.run();
}