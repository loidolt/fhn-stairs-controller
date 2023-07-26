// Serial Communication
//
//


////
// Input Handler Variables
////

// Arcade Button
int arcadeBtnBounceTime = 50;
int arcadeBtnHoldTime = 250;
int arcadeBtnDoubleTime = 500;

int arcadeBtnLastReading = LOW;
int arcadeBtnHold = 0;
int arcadeBtnSingle = 0;

long arcadeBtnLastSwitchTime = 0;
long arcadeBtnOnTime = 0;

// Up Toggle
int upToggleBounceTime = 100;
int upToggleHoldTime = 1000;
int upToggleDoubleTime = 700;

int upToggleLastReading = LOW;
int upToggleHold = 0;
int upToggleSingle = 0;

long upToggleLastSwitchTime = 0;
long upToggleOnTime = 0;

// Down Toggle
int downToggleBounceTime = 100;
int downToggleHoldTime = 1000;
int downToggleDoubleTime = 700;

int downToggleLastReading = LOW;
int downToggleHold = 0;
int downToggleSingle = 0;

long downToggleLastSwitchTime = 0;
long downToggleOnTime = 0;


////
// Sub Loop
////
void input() {
  handleEStopButton();

  if (!emergencyStop) {
    handleArcadeButton();
    handleUpToggle();
    handleDownToggle();
  }
}


////
// Arcade Button
////
void handleArcadeButton() {

  int reading = digitalRead(arcadeBtnPin);

  //first pressed
  if (reading == LOW && arcadeBtnLastReading == HIGH) {
    arcadeBtnOnTime = millis();
  }

  //held
  if (reading == LOW && arcadeBtnLastReading == LOW) {
    if ((millis() - arcadeBtnOnTime) > arcadeBtnHoldTime) {
      arcadeBtnHold = 1;
    }
  }

  //released
  if (reading == HIGH && arcadeBtnLastReading == LOW) {
    if (((millis() - arcadeBtnOnTime) > arcadeBtnBounceTime) && arcadeBtnHold != 1) {
      arcadeButtonOnRelease();
    }
    if (arcadeBtnHold == 1) {
      broadcast("a_h");
      arcadeBtnHold = 0;
    }
  }
  arcadeBtnLastReading = reading;

  if (arcadeBtnSingle == 1 && (millis() - arcadeBtnLastSwitchTime) > arcadeBtnDoubleTime) {
    broadcast("a_s");
    arcadeBtnSingle = 0;
  }
}


void arcadeButtonOnRelease() {

  if ((millis() - arcadeBtnLastSwitchTime) >= arcadeBtnDoubleTime) {
    arcadeBtnSingle = 1;
    arcadeBtnLastSwitchTime = millis();
    return;
  }

  if ((millis() - arcadeBtnLastSwitchTime) < arcadeBtnDoubleTime) {
    broadcast("a_d");
    arcadeBtnSingle = 0;
    arcadeBtnLastSwitchTime = millis();
  }
}


////
// Up Toggle
////
void handleUpToggle() {

  int reading = digitalRead(upTogglePin);

  //first pressed
  if (reading == LOW && upToggleLastReading == HIGH) {
    upToggleOnTime = millis();
  }

  //held
  if (reading == LOW && upToggleLastReading == LOW) {
    if ((millis() - upToggleOnTime) > upToggleHoldTime) {
      if (state == 0 || state == 1) {
        state = 11;
      }
      upToggleHold = 1;
    }
  }

  //released
  if (reading == HIGH && upToggleLastReading == LOW) {
    if (((millis() - upToggleOnTime) > upToggleBounceTime) && upToggleHold != 1) {
      upToggleOnRelease();
    }
    if (upToggleHold == 1) {
      broadcast("u_h");
      upToggleHold = 0;
    }
  }
  upToggleLastReading = reading;

  if (upToggleSingle == 1 && (millis() - upToggleLastSwitchTime) > upToggleDoubleTime) {
    broadcast("u_s");
    upToggleSingle = 0;
  }
}


void upToggleOnRelease() {

  if ((millis() - upToggleLastSwitchTime) >= upToggleDoubleTime) {
    upToggleSingle = 1;
    upToggleLastSwitchTime = millis();
    return;
  }

  if ((millis() - upToggleLastSwitchTime) < upToggleDoubleTime) {
    broadcast("u_d");
    upToggleSingle = 0;
    upToggleLastSwitchTime = millis();
  }
}


////
// Down Toggle
////
void handleDownToggle() {

  int reading = digitalRead(downTogglePin);

  //first pressed
  if (reading == LOW && downToggleLastReading == HIGH) {
    downToggleOnTime = millis();
  }

  //held
  if (reading == LOW && downToggleLastReading == LOW) {
    if ((millis() - downToggleOnTime) > downToggleHoldTime) {
      if (state == 0 || state == 1) {
        state = 12;
      }
      downToggleHold = 1;
    }
  }

  //released
  if (reading == HIGH && downToggleLastReading == LOW) {
    if (((millis() - downToggleOnTime) > downToggleBounceTime) && downToggleHold != 1) {
      downToggleOnRelease();
    }
    if (downToggleHold == 1) {
      broadcast("d_h");
      upToggleHold = 0;
    }
  }
  downToggleLastReading = reading;

  if (downToggleSingle == 1 && (millis() - downToggleLastSwitchTime) > downToggleDoubleTime) {
    broadcast("d_s");
    downToggleSingle = 0;
  }
}


void downToggleOnRelease() {

  if ((millis() - downToggleLastSwitchTime) >= downToggleDoubleTime) {
    downToggleSingle = 1;
    downToggleLastSwitchTime = millis();
    return;
  }

  if ((millis() - downToggleLastSwitchTime) < downToggleDoubleTime) {
    broadcast("d_d");
    downToggleSingle = 0;
    downToggleLastSwitchTime = millis();
  }
}


////
// Emergency Stop
////
void handleEStopButton() {
  int reading = digitalRead(eStopPin);

  if (reading == HIGH) {
    broadcast("e_s");
    emergencyStop = true;
    Controller.println("e_s");
    Serial.println("EMERGENCY STOP!");
  } else if (reading == LOW && terminalState == "scl") {
    emergencyStop = false;
    Controller.println("scl");
    Serial.println("EMERGENCY STOP RESET");
  }
}