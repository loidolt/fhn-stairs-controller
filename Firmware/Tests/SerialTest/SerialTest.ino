////
// Serial Communication
////
#include <SoftwareSerial.h>
const byte rxPin = 13;
const byte txPin = 12;
SoftwareSerial Controller(rxPin, txPin);

////
// Runtime Variables
////
//Serial
const byte numSerialChars = 32;
char receivedSerialChars[numSerialChars];  // an array to store the received data

boolean newSerialData = false;

//Controller
const byte numControllerChars = 32;
char receivedControllerChars[numControllerChars];  // an array to store the received data

boolean newControllerData = false;


////
// Initial Setup
////
void setup() {
  Serial.begin(9600);
  Serial.println("<Arduino is ready>");
}


////
// Main Loop
////
void loop() {
  // Serial Communication Handlers
  recvSerialWithEndMarker();
  showNewSerialData();
  recvControllerWithEndMarker();
  showNewControllerData();
}


////
// Handle Main Serial
////
void recvSerialWithEndMarker() {
  static byte ndx = 0;
  char endMarker = '\n';
  char rc;

  // if (Serial.available() > 0) {
  while (Serial.available() > 0 && newSerialData == false) {
    rc = Serial.read();

    if (rc != endMarker) {
      receivedSerialChars[ndx] = rc;
      ndx++;
      if (ndx >= numSerialChars) {
        ndx = numSerialChars - 1;
      }
    } else {
      receivedSerialChars[ndx] = '\0';  // terminate the string
      ndx = 0;
      newSerialData = true;
    }
  }
}

void showNewSerialData() {
  if (newSerialData == true) {
    Serial.print("Serial: ");
    Serial.println(receivedSerialChars);
    newSerialData = false;
  }
}

////
// Handle Controller Serial
////
void recvControllerWithEndMarker() {
  static byte ndx = 0;
  char endMarker = '\n';
  char rc;

  // if (Serial.available() > 0) {
  while (Controller.available() > 0 && newControllerData == false) {
    rc = Controller.read();

    if (rc != endMarker) {
      receivedControllerChars[ndx] = rc;
      ndx++;
      if (ndx >= numControllerChars) {
        ndx = numControllerChars - 1;
      }
    } else {
      receivedControllerChars[ndx] = '\0';  // terminate the string
      ndx = 0;
      newControllerData = true;
    }
  }
}

void showNewControllerData() {
  if (newControllerData == true) {
    Serial.print("Controller: ");
    Serial.println(receivedControllerChars);
    newControllerData = false;
  }
}