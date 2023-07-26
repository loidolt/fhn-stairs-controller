// Bounce.pde
// -*- mode: C++ -*-
//
// Make a single stepper bounce from one limit to another
//
// Copyright (C) 2012 Mike McCauley
// $Id: Random.pde,v 1.1 2011/01/05 01:51:01 mikem Exp mikem $

#include <AccelStepper.h>

const int retractedEndstopPin = 2;
const int extendedEndstopPin = 3;
const int stepperPulsePin = 5;
const int stepperDirectionPin = 6;
const int stepperEnablePin = 7;
const int stepperAlarmPin = 8;

bool homed = false;
const int maxSpeed = 1000;
const int homeSpeed = 250;
const int acceleration = 100;
const int maxDist = 938;       //mm
const int endstopBackoff = 15;  //mm

// Define a stepper and the pins it will use
// not using default 4 wires setup, but instead using step, direction, and enable pins
AccelStepper stepper(AccelStepper::DRIVER, stepperPulsePin, stepperDirectionPin);

void setup() {
  delay(1000);
  Serial.begin(9600);

  pinMode(retractedEndstopPin, INPUT_PULLUP);
  pinMode(extendedEndstopPin, INPUT_PULLUP);

  // Change these to suit your stepper if you want
  stepper.setEnablePin(stepperEnablePin);
  stepper.setPinsInverted(false, false, true);
  stepper.enableOutputs();
  stepper.setAcceleration(acceleration);

  homeMachine();

  stepper.setMaxSpeed(maxSpeed);
}

void loop() {
  retractMachine();
  extendMachine();

  if (stepper.distanceToGo() == 0) {
    stepper.disableOutputs();
  }
}

long distToSteps(int distance) {
  Serial.println(distance);
  float stepsToMM = (200 * 1) / 5;
  long convertedDistance = distance * stepsToMM;
  Serial.println(convertedDistance);
  return convertedDistance;
}

void retractMachine() {
  stepper.moveTo(0);
  while (stepper.distanceToGo() != 0) {
    stepper.run();
  }
}

void extendMachine() {
  stepper.moveTo(distToSteps(maxDist));
  while (stepper.distanceToGo() != 0) {
    stepper.run();
  }
}

void homeMachine() {
  Serial.println("homing machine");

  stepper.setMaxSpeed(homeSpeed);
  stepper.setCurrentPosition(0);
  Serial.println(stepper.currentPosition());
  stepper.moveTo(distToSteps(maxDist + 10));

  Serial.println("moving to endstop");
  while (!homed) {
    stepper.run();

    if (digitalRead(extendedEndstopPin) == HIGH) {
      Serial.println("endstop hit");

      stepper.setCurrentPosition(distToSteps(maxDist));
      stepper.moveTo(stepper.currentPosition() - distToSteps(endstopBackoff));

      while (stepper.distanceToGo() != 0) {
        stepper.run();
      }

      stepper.setCurrentPosition(distToSteps(maxDist));
      stepper.moveTo(stepper.currentPosition());
      Serial.println(stepper.currentPosition());

      homed = true;
    }
  }
  Serial.println("home complete");
}