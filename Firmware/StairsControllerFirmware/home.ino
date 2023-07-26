// Homing and Manual Override
//
//


////
// Sub Loop
////
void home() {
  homeAllMotion();
  homeLockingBarMotion();
  homeStairsMotion();
}

////
// Coordinate Safe Homing Procedure
///

void homeAllMotion() {
  if (!homed && homeNow) {
    if (lockingBarState != "hom" && lockingBarState != "ret") {
      Serial.println("HOME LOCKING BAR");
      homeLockingBar = true;
      homeStairs = false;
    } else if (lockingBarState == "hom") {
      Serial.println("RETRACT LOCKING BAR");
      homeLockingBar = false;
      retractLockingBar = true;
    } else if (lockingBarState != "hom" && lockingBarState == "ret") {
      Serial.println("HOME STAIRS");
      homeLockingBar = false;
      homeStairs = true;
    }
  }
}

// Home Locking Bar and Retract for safe stair homing
void homeLockingBarMotion() {
  if (homeLockingBar && !stairsRaised) {
    if (lockingBarState == "ext") {
      lockingBarExtended = true;
      Serial.println("HOMED");
    }
    if (lockingBarState != "hom" && lockingBarState != "ext") {
      lockingBarMillis = millis();
      lockingBarExtended = false;
      rateLimitBarSerial("ret");
      broadcastNodes("mov");
    }
    if (millis() - lockingBarMillis > retractDuration && lockingBarState != "ext") {  //request locking bar state if extended is not confirmed
      lockingBarExtended = false;
      rateLimitBarSerial("ste");
    }
  } else if (homeLockingBar && stairsRaised) {
    if (hoistState != 1) {
      hoistMillis = millis();
      hoistDown();
    }
    if (millis() - hoistMillis > safeStairsLowerDuration) {  //lower for interval so locking bar can clear stairs
      hoistStop();
      stairsRaised = false;
    }
  }
}

// Home Stairs
void homeStairsMotion() {
  if (homeStairs && !lockingBarExtended) {
    liftStairs = true;
  }
}