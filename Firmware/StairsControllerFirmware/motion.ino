// Motion Systems
//
//


////
// Sub Loop
////
void motion() {
  upEndstopReset();
  downEndstopReset();

  liftStairsMotion();
  lowerStairsMotion();
  parkStairsMotion();
  unparkStairsMotion();

  extendLockingBarMotion();
  retractLockingBarMotion();
}


////
// Stairs Motion (Hoist)
////
void liftStairsMotion() {
  if (liftStairs && !lockingBarExtended) {
    if (hoistState != 1) {
      hoistMillis = millis();
      hoistUp();
    }
    if (millis() - hoistMillis > liftDuration) {  //lift for interval
      hoistStop();
      liftStairs = false;
      parkStairs = true;
      stairsRaised = true;
    }
  } else if (liftStairs && lockingBarExtended) {
    retractLockingBar = true;
  }
}

void lowerStairsMotion() {
  if (lowerStairs && !lockingBarExtended) {
    if (hoistState != 2) {
      hoistMillis = millis();
      hoistDown();
    }
    if (millis() - hoistMillis > lowerDuration) {  //lift for interval
      hoistStop();
      lowerStairs = false;
      stairsRaised = false;
    }
  } else if (lowerStairs && lockingBarExtended) {
    unparkStairs = true;
  }
}

void parkStairsMotion() {
  if (parkStairs && lockingBarExtended) {
    if (hoistState != 2) {
      hoistMillis = millis();
      hoistDown();
    }
    if (millis() - hoistMillis > lowerDuration) {  //lift for interval or stop if endstop triggered
      hoistStop();
      parkStairs = false;
      Serial.println("PARKED");
    }
  } else if (parkStairs && !lockingBarExtended) {
    extendLockingBar = true;
  }
}

void unparkStairsMotion() {
  if (unparkStairs) {
    if (hoistState != 1) {
      hoistMillis = millis();
      hoistUp();
    }
    if (millis() - hoistMillis > unparkDuration) {  //lift for interval or stop if endstop triggered
      hoistStop();
      unparkStairs = false;
      retractLockingBar = true;
      Serial.println("UNPARKED");
    }
  }
}


////
// Locking Bar Motion
////
void extendLockingBarMotion() {
  if (extendLockingBar) {
    if (lockingBarState == "ext") {
      lockingBarExtended = true;
      Serial.println("EXTENDED");
      broadcastNodes("ext");
    }
    if (lockingBarState != "mov" && lockingBarState != "ext") {
      lockingBarMillis = millis();
      lockingBarExtended = false;
      rateLimitBarSerial("ext");
      broadcastNodes("mov");
    }
    if (millis() - lockingBarMillis > extendDuration && lockingBarState != "ext") {  //request locking bar state if extended is not confirmed
      lockingBarExtended = false;
      rateLimitBarSerial("ste");
    }
  }
}

void retractLockingBarMotion() {
  if (retractLockingBar) {
    if (lockingBarState == "ret") {
      lockingBarExtended = false;
      Serial.println("RETRACTED");
      broadcastNodes("ret");
    }
    if (lockingBarState != "mov" && lockingBarState != "ret") {
      lockingBarMillis = millis();
      lockingBarExtended = true;
      rateLimitBarSerial("ret");
      broadcastNodes("mov");
    }
    if (millis() - lockingBarMillis > retractDuration && lockingBarState != "ret") {  //request locking bar state if retracted is not confirmed
      lockingBarExtended = true;
      rateLimitBarSerial("ste");
    }
  }
}


////
// Endstop Backoff and Reset
////
void upEndstopReset() {
  if (upEndstopTriggered) {
    if (hoistState != 2) {
      hoistMillis = millis();
      hoistDown();
    }
    if (millis() - hoistMillis > endstopBackoffDuration) {  //lift for interval or stop if endstop triggered
      hoistStop();
      upEndstopTriggered = false;
      stairsRaised = true;
    }
  }
}

void downEndstopReset() {
  if (downEndstopTriggered) {
    if (hoistState != 1) {
      hoistMillis = millis();
      hoistUp();
    }
    if (millis() - hoistMillis > endstopBackoffDuration) {  //lift for interval or stop if endstop triggered
      hoistStop();
      downEndstopTriggered = false;
      stairsRaised = false;
    }
  }
}


////
// Hoist Relay Bonding
////
void hoistStop() {
  Serial.println("HOIST STOP");
  digitalWrite(hoist1, LOW);
  digitalWrite(hoist2, LOW);
  hoistState = 0;
}

void hoistUp() {
  Serial.println("HOIST UP");
  digitalWrite(hoist1, HIGH);
  digitalWrite(hoist2, LOW);
  broadcast("22");
  hoistState = 1;
}

void hoistDown() {
  Serial.println("HOIST DOWN");
  digitalWrite(hoist1, LOW);
  digitalWrite(hoist2, HIGH);
  broadcast("22");
  hoistState = 2;
}