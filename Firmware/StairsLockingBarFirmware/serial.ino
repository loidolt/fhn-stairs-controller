// Serial Communication
//
//

////
// Command Mapping
////
// ID  -  CODE |  Purpose
//------------------------------------------------
// 0   -  a_s  |  Arcade Button Single Press
// 1   -  a_d  |  Arcade Button Double Press
// 2   -  a_h  |  Arcade Button Held
// 3   -  u_s  |  Up Toggle Single Press
// 4   -  u_d  |  Up Toggle Double Press
// 5   -  u_h  |  Up Toggle Held
// 6   -  d_s  |  Down Toggle Single Press
// 7   -  d_d  |  Down Toggle Double Press
// 8   -  d_h  |  Down Toggle Held
// 9   -  e_s  |  Emergency Stop Press
// 10  -  stp  |  Emergency Stop State
// 11  -  ste  |  Request Current State
// 12  -  mov  |  System In Motion
// 13  -  r_u  |  Stairs Raised and Unparked
// 14  -  r_p  |  Stairs Raised and Parked
// 15  -  l_e  |  Stairs Lowered and Bar Extended
// 16  -  l_r  |  Stairs Lowered and Bar Retracted
// 17  -  ext  |  Bar Extended
// 18  -  ret  |  Bar Retracted
// 19  -  unk  |  Position Unknown
// 20  -  h_n  |  Home Machine Now
// 21  -  h_m  |  Home Procedure Movement
// 22  -  ovr  |  Manual Movement Override
// 23  -  o_h  |  Manual Movement Hoist Mode
// 24  -  o_b  |  Manual Movement Bar Mode
// 25  -  h_u  |  Manual Movement Hoist Up
// 26  -  h_d  |  Manual Movement Hoist Down
// 27  -  b_e  |  Manual Movement Bar Extend
// 28  -  b_r  |  Manual Movement Bar Retract
// 30  -  rok  |  Received Command Successfully
// 31  -  scl  |  Clear Emergency Stop


////
// Runtime Variables
////

//Rate Limited Messages
const int requestRateLimit = 100;

unsigned long serialRateMillis = 0;
String serialRLLastMessage = "";
unsigned long controllerRateMillis = 0;
String controllerRLLastMessage = "";

// Serial
const byte numSerialChars = 32;
char receivedSerialChars[numSerialChars];  // an array to store the received data

boolean newSerialData = false;

// Controller
const byte numControllerChars = 32;
char receivedControllerChars[numControllerChars];  // an array to store the received data

boolean newControllerData = false;

////
// Sub Loop
////
void serial() {

  recvSerialWithEndMarker();
  recvControllerWithEndMarker();

  handleNewData();
}


////
// Event Handlers
////

void handleNewData() {
  if (newSerialData == true) {
    handleExternalInput(0, receivedSerialChars);
    newSerialData = false;
  }

  if (newControllerData == true) {
    handleExternalInput(1, receivedControllerChars);
    newControllerData = false;
  }
}

void handleExternalInput(int origin, String extCommand) {
  if (extCommand.equals("stp") || extCommand.equals("10")) {
    emergencyStop = true;
    routeStateChange(origin, "10");
  } else if (extCommand.equals("h_n") || extCommand.equals("20")) {
    homeMachine = true;
    routeStateChange(origin, "h_n");
  } else if (extCommand.equals("ste") || extCommand.equals("11")) {
    reportState = true;
    routeStateChange(origin, "ste");
  } else if (extCommand.equals("ext") || extCommand.equals("17")) {
    extendMotion();
    routeStateChange(origin, "ext");
  } else if (extCommand.equals("ret") || extCommand.equals("18")) {
    retractMotion();
    routeStateChange(origin, "ret");
  } else if (extCommand.equals("b_e") || extCommand.equals("25")) {
    manualExtendMotion();
    routeStateChange(origin, "b_e");
  } else if (extCommand.equals("b_r") || extCommand.equals("26")) {
    manualRetractMotion();
    routeStateChange(origin, "b_r");
  } else {
    Serial.println("inv");
  }
}

void routeStateChange(int origin, String input) {
  switch (origin) {
    case 0:
      Serial.print("Serial: ");
      Serial.println(input);
      terminalState = input;
      break;
    case 1:
      Serial.print("Controller: ");
      Serial.println(input);
      controllerState = input;
      break;
  }
}

void handleReportState() {
  Controller.println("unk");
  reportState = false;
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

void rateLimitSerial(String message) {
  if (serialRLLastMessage != message) {
    serialRateMillis = millis();
    serialRLLastMessage = message;
  }
  if (millis() - serialRateMillis > requestRateLimit) {  //limit state request
    Serial.println(message);
    serialRLLastMessage = "";
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

void rateLimitControllerSerial(String message) {
  if (controllerRLLastMessage != message) {
    controllerRateMillis = millis();
    controllerRLLastMessage = message;
  }
  if (millis() - controllerRateMillis > requestRateLimit) {  //limit state request
    Controller.println(message);
    controllerRLLastMessage = "";
  }
}