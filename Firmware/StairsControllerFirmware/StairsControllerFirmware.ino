// Stairs Controller Firmware v1
// Chris Loidolt
// 02/21/2023


////
// Serial Communication
////
// Arduino Mega Serial Pins
// Serial         -  0(RX), 1(TX) (reserved for programming and debugging)
// Serial1        -  19(RX), 18(TX) (Locking Bar)
// Serial2        -  17(RX), 16(TX) (Control Node 1)
// Serial3        -  15(RX), 14(TX) (Control Node 2)
// SoftwareSerial -  13(RX), 12(TX) (Control Node 3)
#include <SoftwareSerial.h>
HardwareSerial& Bar = Serial1;
HardwareSerial& Node1 = Serial2;
HardwareSerial& Node2 = Serial3;

// Software Serial Pins
const byte rxPin = 13;
const byte txPin = 12;
SoftwareSerial Node3(rxPin, txPin);


// Endstops
const byte upEndstopPin = 2;    //interupt
const byte downEndstopPin = 3;  //interupt
bool upEndstopTriggered = false;
bool downEndstopTriggered = false;


// Hoist
const byte hoist1 = 25;
const byte hoist2 = 26;
const byte relay3 = 27;

////
// Runtime Variables
////
String command;
int state = 30;
String terminalState = "unk";
String lockingBarState = "unk";
String controlNode1State = "unk";
String controlNode2State = "unk";
String controlNode3State = "unk";


////
// Motion Runtime Variables
////
bool emergencyStop = false;
bool moving = false;
bool liftStairs = false;
bool lowerStairs = false;
bool parkStairs = false;
bool unparkStairs = false;
bool extendLockingBar = false;
bool retractLockingBar = false;

// Hoist
unsigned long hoistMillis = 0;
int hoistState = 0;
const int liftDuration = 5000;
const int lowerDuration = 5000;
const int parkDuration = 2000;
const int unparkDuration = 2000;
const int endstopBackoffDuration = 500;
const int safeStairsLowerDuration = 10000;
bool stairsRaised = false;

//Locking Bar
unsigned long lockingBarMillis = 0;
const int extendDuration = 60000;
const int retractDuration = 60000;
bool lockingBarExtended = false;


////
// Home Runtime Variables
////
bool homed = false;
bool homeNow = false;
bool manualOverride = false;
bool overrideBar = false;

bool homeLockingBar = false;
bool homeStairs = false;

////
// Initial Setup
////
void setup() {
  delay(2000);

  // Define pin modes for softwareserial TX and RX
  pinMode(rxPin, INPUT);
  pinMode(txPin, OUTPUT);

  Serial.begin(115200);
  Bar.begin(115200);
  Node1.begin(115200);
  Node2.begin(115200);
  Node3.begin(115200);

  // Endstops
  pinMode(upEndstopPin, INPUT_PULLUP);
  pinMode(downEndstopPin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(upEndstopPin), upEndstop, RISING);
  attachInterrupt(digitalPinToInterrupt(downEndstopPin), downEndstop, RISING);

  // Hoist Relays
  pinMode(hoist1, OUTPUT);
  pinMode(hoist2, OUTPUT);
  pinMode(relay3, OUTPUT);
  digitalWrite(hoist1, LOW);
  digitalWrite(hoist2, LOW);
  digitalWrite(relay3, LOW);

  Serial.println("Stairs Controller");

  delay(200);
}


////
// Main Loop
////
void loop() {
  serial();
  motion();
  home();
}


////
// Endstop Interrupts
////
void upEndstop() {
  upEndstopTriggered = true;
  Serial.println("UP ENDSTOP HIT");
}

void downEndstop() {
  downEndstopTriggered = true;
  Serial.println("DOWN ENDSTOP HIT");
}
