// Stairs Locking Bar Firmware v2
// Chris Loidolt
// 02/24/2023

// Aviation Connector Wiring
// 1 - +24V
// 2 - GND
// 3 - RX
// 4 - TX

// Stepper Driver Configuration
// Max 200Khz input frequency
// PUL duty cycle recommended 50% miniumum
// 24V input power default
// Enable LOW, Disable HIGH
// ALARM NO during use, NC during error


////
// Serial Communication
////
#include <SoftwareSerial.h>
// Software Serial Pins
const byte rxPin = 10;
const byte txPin = 11;
SoftwareSerial Controller(rxPin, txPin);


////
// Stepper Motor
////
#include <AccelStepper.h>
const byte stepperPulsePin = 5;
const byte stepperDirectionPin = 6;
const byte stepperEnablePin = 7;
const byte stepperAlarmPin = 8;
// not using default 4 wires setup, but instead using step, direction, and enable pins
AccelStepper stepper(AccelStepper::DRIVER, stepperPulsePin, stepperDirectionPin);

const int maxSpeed = 1000;
const int homeSpeed = 250;
const int acceleration = 100;

// Endstops
const byte retractedEndstopPin = 3;
const byte extendedEndstopPin = 2;


////
// Runtime Variables
////
String state = "unk";
String terminalState = "unk";
String controllerState = "unk";

// Motion
bool homeMachine = false;
bool emergencyStop = false;
bool reportState = false;


////
// Initial Setup
////
void setup() {
  delay(2000);

  Serial.begin(115200);

  // Define pin modes for TX and RX
  pinMode(rxPin, INPUT);
  pinMode(txPin, OUTPUT);

  Controller.begin(115200);

  pinMode(retractedEndstopPin, INPUT_PULLUP);
  pinMode(extendedEndstopPin, INPUT_PULLUP);

  // Change these to suit your stepper if you want
  stepper.setEnablePin(stepperEnablePin);
  stepper.setPinsInverted(false, false, true);
  stepper.setAcceleration(acceleration);

  handleReportState();
}

////
// Main Loop
////
void loop() {
  mandatory();
  motion();

  //delay(200);
}


////
// Mandatory Functions
///
void mandatory() {
  serial();
  if (emergencyStop) {
    handleStopMotion();
    reportState = true;
  }
  if (reportState) {
    handleReportState();
  }
}