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
const int requestRateLimit = 200;

unsigned long serialRateMillis = 0;
String serialRLLastMessage = "";
unsigned long barRateMillis = 0;
String barRLLastMessage = "";
unsigned long node1RateMillis = 0;
String node1RLLastMessage = "";
unsigned long node2RateMillis = 0;
String node2RLLastMessage = "";
unsigned long node3RateMillis = 0;
String node3RLLastMessage = "";

// Serial
const byte numSerialChars = 32;
char receivedSerialChars[numSerialChars];  // an array to store the received data

boolean newSerialData = false;

// Bar
const byte numBarChars = 32;
char receivedBarChars[numBarChars];  // an array to store the received data

boolean newBarData = false;

// Node1
const byte numNode1Chars = 32;
char receivedNode1Chars[numNode1Chars];  // an array to store the received data

boolean newNode1Data = false;

// Node3
const byte numNode2Chars = 32;
char receivedNode2Chars[numNode2Chars];  // an array to store the received data

boolean newNode2Data = false;

// Node3
const byte numNode3Chars = 32;
char receivedNode3Chars[numNode3Chars];  // an array to store the received data

boolean newNode3Data = false;

////
// Sub Loop
////
void serial() {

  recvSerialWithEndMarker();
  recvBarWithEndMarker();
  recvNode1WithEndMarker();
  recvNode2WithEndMarker();
  recvNode3WithEndMarker();

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

  if (newBarData == true) {
    handleExternalInput(1, receivedBarChars);
    newBarData = false;
  }

  if (newNode1Data == true) {
    handleExternalInput(2, receivedNode1Chars);
    newNode1Data = false;
  }

  if (newNode2Data == true) {
    handleExternalInput(3, receivedNode2Chars);
    newNode2Data = false;
  }

  if (newNode3Data == true) {
    handleExternalInput(4, receivedNode3Chars);
    newNode3Data = false;
  }
}

void handleExternalInput(int origin, String extCommand) {
  if (extCommand.equals("a_s") || extCommand.equals("0")) {
    if (!homed) {
      if (manualOverride) {
        overrideBar = !overrideBar;
      } else {
        manualOverride = true;
      }
    }
    routeStateChange(origin, "a_s");
  } else if (extCommand.equals("a_d") || extCommand.equals("1")) {
    routeStateChange(origin, "a_d");
  } else if (extCommand.equals("a_h") || extCommand.equals("2")) {
    if (!homed) {
      manualOverride = true;
      broadcast("ovr");
    }
    routeStateChange(origin, "a_h");
  } else if (extCommand.equals("u_s") || extCommand.equals("3")) {
    routeStateChange(origin, "u_s");
  } else if (extCommand.equals("u_d") || extCommand.equals("4")) {
    routeStateChange(origin, "u_d");
  } else if (extCommand.equals("u_h") || extCommand.equals("5")) {
    if (!manualOverride) {
      liftStairs = true;
    } else if (manualOverride && !overrideBar) {
      hoistDown();
      broadcastNodes("h_u");
    } else if (manualOverride && overrideBar) {
      Bar.println("b_r");
      broadcastNodes("b_e");
    }
    routeStateChange(origin, "u_h");
  } else if (extCommand.equals("d_s") || extCommand.equals("6")) {
    routeStateChange(origin, "d_s");
  } else if (extCommand.equals("d_d") || extCommand.equals("7")) {
    routeStateChange(origin, "d_d");
  } else if (extCommand.equals("d_h") || extCommand.equals("8")) {
    if (!manualOverride) {
      lowerStairs = true;
    } else if (manualOverride && !overrideBar) {
      hoistDown();
      broadcastNodes("h_d");
    } else if (manualOverride && overrideBar) {
      Bar.println("b_r");
      broadcastNodes("b_r");
    }
    routeStateChange(origin, "d_h");
  } else if (extCommand.equals("e_s") || extCommand.equals("9")) {
    emergencyStop = true;
    routeStateChange(origin, "e_s");
  } else if (extCommand.equals("ste") || extCommand.equals("11")) {
    routeStateChange(origin, "ste");
  } else if (extCommand.equals("mov") || extCommand.equals("12")) {
    moving = true;
    broadcastNodes("mov");
    routeStateChange(origin, "mov");
  } else if (extCommand.equals("r_u") || extCommand.equals("13")) {
    broadcastNodes("r_u");
    routeStateChange(origin, "r_u");
  } else if (extCommand.equals("r_p") || extCommand.equals("14")) {
    broadcastNodes("r_p");
    routeStateChange(origin, "r_p");
  } else if (extCommand.equals("l_e") || extCommand.equals("15")) {
    broadcastNodes("l_e");
    routeStateChange(origin, "l_e");
  } else if (extCommand.equals("l_r") || extCommand.equals("16")) {
    broadcastNodes("l_r");
    routeStateChange(origin, "l_r");
  } else if (extCommand.equals("ext") || extCommand.equals("17")) {
    broadcastNodes("ext");
    routeStateChange(origin, "ext");
  } else if (extCommand.equals("ret") || extCommand.equals("18")) {
    broadcastNodes("ret");
    routeStateChange(origin, "ret");
  } else if (extCommand.equals("unk") || extCommand.equals("19")) {
    homed = false;
    broadcastNodes("unk");
    routeStateChange(origin, "unk");
  } else if (extCommand.equals("h_n") || extCommand.equals("20")) {
    homeNow = true;
    routeStateChange(origin, "h_n");
  } else if (extCommand.equals("h_m") || extCommand.equals("21")) {
    broadcastNodes("h_m");
    routeStateChange(origin, "h_m");
  } else if (extCommand.equals("ovr") || extCommand.equals("22")) {
    broadcast("ovr");
    routeStateChange(origin, "ovr");
  } else if (extCommand.equals("o_h") || extCommand.equals("23")) {
    overrideBar = false;
    broadcastNodes("o_h");
    routeStateChange(origin, "o_h");
  } else if (extCommand.equals("o_b") || extCommand.equals("24")) {
    overrideBar = true;
    broadcastNodes("o_b");
    routeStateChange(origin, "o_b");
  } else if (extCommand.equals("rok") || extCommand.equals("30")) {
    routeStateChange(origin, "rok");
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
      broadcast(input);
      break;
    case 1:
      Serial.print("Bar: ");
      Serial.println(input);
      lockingBarState = input;
      broadcast(input);
      break;
    case 2:
      Serial.print("Node1: ");
      Serial.println(input);
      controlNode1State = input;
      broadcast(input);
      break;
    case 3:
      Serial.print("Node2: ");
      Serial.println(input);
      controlNode2State = input;
      broadcast(input);
      break;
    case 4:
      Serial.print("Node3: ");
      Serial.println(input);
      controlNode3State = input;
      broadcast(input);
      break;
  }
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
// Handle Bar Serial
////
void recvBarWithEndMarker() {
  static byte ndx = 0;
  char endMarker = '\n';
  char rc;

  // if (Serial.available() > 0) {
  while (Bar.available() > 0 && newBarData == false) {
    rc = Bar.read();

    if (rc != endMarker) {
      receivedBarChars[ndx] = rc;
      ndx++;
      if (ndx >= numBarChars) {
        ndx = numBarChars - 1;
      }
    } else {
      receivedBarChars[ndx] = '\0';  // terminate the string
      ndx = 0;
      newBarData = true;
    }
  }
}

void rateLimitBarSerial(String message) {
  if (barRLLastMessage != message) {
    barRateMillis = millis();
    barRLLastMessage = message;
  }
  if (millis() - barRateMillis > requestRateLimit) {  //limit state request
    Bar.println(message);
    barRLLastMessage = "";
  }
}

////
// Handle Node1 Serial
////
void recvNode1WithEndMarker() {
  static byte ndx = 0;
  char endMarker = '\n';
  char rc;

  // if (Serial.available() > 0) {
  while (Node1.available() > 0 && newNode1Data == false) {
    rc = Node1.read();

    if (rc != endMarker) {
      receivedNode1Chars[ndx] = rc;
      ndx++;
      if (ndx >= numNode1Chars) {
        ndx = numNode1Chars - 1;
      }
    } else {
      receivedNode1Chars[ndx] = '\0';  // terminate the string
      ndx = 0;
      newNode1Data = true;
    }
  }
}

void rateLimitNode1Serial(String message) {
  if (node1RLLastMessage != message) {
    node1RateMillis = millis();
    node1RLLastMessage = message;
  }
  if (millis() - node1RateMillis > requestRateLimit) {  //limit state request
    Node1.println(message);
    node1RLLastMessage = "";
  }
}

////
// Handle Node2 Serial
////
void recvNode2WithEndMarker() {
  static byte ndx = 0;
  char endMarker = '\n';
  char rc;

  // if (Serial.available() > 0) {
  while (Node2.available() > 0 && newNode2Data == false) {
    rc = Node2.read();

    if (rc != endMarker) {
      receivedNode2Chars[ndx] = rc;
      ndx++;
      if (ndx >= numNode2Chars) {
        ndx = numNode2Chars - 1;
      }
    } else {
      receivedNode2Chars[ndx] = '\0';  // terminate the string
      ndx = 0;
      newNode2Data = true;
    }
  }
}

void rateLimitNode2Serial(String message) {
  if (node2RLLastMessage != message) {
    node2RateMillis = millis();
    node2RLLastMessage = message;
  }
  if (millis() - node2RateMillis > requestRateLimit) {  //limit state request
    Node2.println(message);
    node2RLLastMessage = "";
  }
}

////
// Handle Node3 Serial
////
void recvNode3WithEndMarker() {
  static byte ndx = 0;
  char endMarker = '\n';
  char rc;

  // if (Serial.available() > 0) {
  while (Node3.available() > 0 && newNode3Data == false) {
    rc = Node3.read();

    if (rc != endMarker) {
      receivedNode3Chars[ndx] = rc;
      ndx++;
      if (ndx >= numNode3Chars) {
        ndx = numNode3Chars - 1;
      }
    } else {
      receivedNode3Chars[ndx] = '\0';  // terminate the string
      ndx = 0;
      newNode3Data = true;
    }
  }
}

void rateLimitNode3Serial(String message) {
  if (node3RLLastMessage != message) {
    node3RateMillis = millis();
    node3RLLastMessage = message;
  }
  if (millis() - node3RateMillis > requestRateLimit) {  //limit state request
    Node3.println(message);
    node3RLLastMessage = "";
  }
}


void broadcastNodes(String message) {
  Node1.println(message);
  Node2.println(message);
  Node3.println(message);
}

void broadcast(String message) {
  Bar.println(message);
  Node1.println(message);
  Node2.println(message);
  Node3.println(message);
}