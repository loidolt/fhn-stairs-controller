// Motion Systems
//
//


////
// Runtime Variables
////
const int maxDist = 938;        //mm
const int endstopBackoff = 15;  //mm


////
// Sub Loop
////
void motion() {
  homeMachineMotion();

  if (stepper.distanceToGo() == 0) {
    stepper.disableOutputs();
  }
}

long distToSteps(int distance) {
  float stepsToMM = (200 * 1) / 5;
  long convertedDistance = distance * stepsToMM;
  return convertedDistance;
}

void handleStopMotion() {
  stepper.moveTo(stepper.currentPosition());
  stepper.disableOutputs();
  state = "unk";
  handleReportState();
}

void retractMotion() {
  state = "mov";
  handleReportState();

  stepper.enableOutputs();
  stepper.moveTo(0);
  while (stepper.distanceToGo() != 0) {
    mandatory();
    stepper.run();
  }

  if (stepper.currentPosition() == 0) {
    state = "ret";
    handleReportState();
  }
}

void manualRetractMotion() {
  state = "mov";
  handleReportState();

  stepper.enableOutputs();
  stepper.moveTo(0);
  while (stepper.distanceToGo() != 0) {
    mandatory();
    stepper.run();
  }

  state = "unk";
  handleReportState();
}

void extendMotion() {
  state = "mov";
  handleReportState();

  stepper.enableOutputs();
  stepper.moveTo(distToSteps(maxDist));
  while (stepper.distanceToGo() != 0) {
    mandatory();
    stepper.run();
  }

  if (stepper.currentPosition() == distToSteps(maxDist)) {
    state = "ext";
    handleReportState();
  }
}

void manualExtendMotion() {
  state = "mov";
  handleReportState();

  stepper.enableOutputs();
  stepper.moveTo(distToSteps(20));
  while (stepper.distanceToGo() != 0) {
    mandatory();
    stepper.run();
  }

  state = "unk";
  handleReportState();
}

void homeMachineMotion() {
  if (homeMachine) {
    Serial.println("HOMING");
    state = "h_m";
    handleReportState();

    stepper.enableOutputs();
    stepper.setMaxSpeed(homeSpeed);
    stepper.setCurrentPosition(0);
    Serial.println(stepper.currentPosition());
    stepper.moveTo(distToSteps(maxDist + 10));

    Serial.println("MOVING TO ENDSTOP");
    while (state != "unk") {
      mandatory();
      stepper.run();

      if (digitalRead(extendedEndstopPin) == LOW) {
        Serial.println("ENDSTOP HIT");

        stepper.setCurrentPosition(distToSteps(maxDist));
        stepper.moveTo(stepper.currentPosition() - distToSteps(endstopBackoff));

        while (stepper.distanceToGo() != 0) {
          stepper.run();
        }

        stepper.setCurrentPosition(distToSteps(maxDist));
        stepper.moveTo(stepper.currentPosition());
        stepper.setMaxSpeed(maxSpeed);
        Serial.println(stepper.currentPosition());

        state = "ext";
      }
    }
  }

  Serial.println("HOME COMPLETE");
  handleReportState();
}