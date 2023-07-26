const int led = 13;


// Arcade Button
const byte arcadeBtnPin = 8;

int arcadeBtnBounceTime = 50;
int arcadeBtnHoldTime = 250;
int arcadeBtnDoubleTime = 500;

int arcadeBtnLastReading = LOW;
int arcadeBtnHold = 0;
int arcadeBtnSingle = 0;

long arcadeBtnLastSwitchTime = 0;
long arcadeBtnOnTime = 0;

// Up Toggle
const byte upTogglePin = 6;

int upToggleBounceTime = 50;
int upToggleHoldTime = 250;
int upToggleDoubleTime = 500;

int upToggleLastReading = LOW;
int upToggleHold = 0;
int upToggleSingle = 0;

long upToggleLastSwitchTime = 0;
long upToggleOnTime = 0;

// Down Toggle
const byte downTogglePin = 7;

int downToggleBounceTime = 50;
int downToggleHoldTime = 250;
int downToggleDoubleTime = 500;

int downToggleLastReading = LOW;
int downToggleHold = 0;
int downToggleSingle = 0;

long downToggleLastSwitchTime = 0;
long downToggleOnTime = 0;


int LEDstate = 0;


void setup() {
  pinMode(arcadeBtnPin, INPUT_PULLUP);
  pinMode(upTogglePin, INPUT_PULLUP);
  pinMode(downTogglePin, INPUT_PULLUP);
  pinMode(led, OUTPUT);
  digitalWrite(led, LOW);
  Serial.begin(9600);
}

void loop() {
  handleArcadeButton();
  handleUpToggle();
  handleDownToggle();
}

void handleArcadeButton() {

  int reading = digitalRead(arcadeBtnPin);

  //first pressed
  if (reading == LOW && arcadeBtnLastReading == HIGH) {
    arcadeBtnOnTime = millis();
  }

  //held
  if (reading == LOW && arcadeBtnLastReading == LOW) {
    if ((millis() - arcadeBtnOnTime) > arcadeBtnHoldTime) {
      invertLED();
      arcadeBtnHold = 1;
    }
  }

  //released
  if (reading == HIGH && arcadeBtnLastReading == LOW) {
    if (((millis() - arcadeBtnOnTime) > arcadeBtnBounceTime) && arcadeBtnHold != 1) {
      arcadeButtonOnRelease();
    }
    if (arcadeBtnHold == 1) {
      Serial.println("arcade held");
      digitalWrite(led, LEDstate);
      arcadeBtnHold = 0;
    }
  }
  arcadeBtnLastReading = reading;

  if (arcadeBtnSingle == 1 && (millis() - arcadeBtnLastSwitchTime) > arcadeBtnDoubleTime) {
    Serial.println("arcade single press");
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
    toggleLED();
    Serial.println("arcade double press");
    arcadeBtnSingle = 0;
    arcadeBtnLastSwitchTime = millis();
  }
}

void handleUpToggle() {

  int reading = digitalRead(upTogglePin);

  //first pressed
  if (reading == LOW && upToggleLastReading == HIGH) {
    upToggleOnTime = millis();
  }

  //held
  if (reading == LOW && upToggleLastReading == LOW) {
    if ((millis() - upToggleOnTime) > upToggleHoldTime) {
      invertLED();
      upToggleHold = 1;
    }
  }

  //released
  if (reading == HIGH && upToggleLastReading == LOW) {
    if (((millis() - upToggleOnTime) > upToggleBounceTime) && upToggleHold != 1) {
      upToggleOnRelease();
    }
    if (upToggleHold == 1) {
      Serial.println("up held");
      digitalWrite(led, LEDstate);
      upToggleHold = 0;
    }
  }
  upToggleLastReading = reading;

  if (upToggleSingle == 1 && (millis() - upToggleLastSwitchTime) > upToggleDoubleTime) {
    Serial.println("up single press");
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
    toggleLED();
    Serial.println("up double press");
    upToggleSingle = 0;
    upToggleLastSwitchTime = millis();
  }
}

void handleDownToggle() {

  int reading = digitalRead(downTogglePin);

  //first pressed
  if (reading == LOW && downToggleLastReading == HIGH) {
    downToggleOnTime = millis();
  }

  //held
  if (reading == LOW && downToggleLastReading == LOW) {
    if ((millis() - downToggleOnTime) > downToggleHoldTime) {
      invertLED();
      downToggleHold = 1;
    }
  }

  //released
  if (reading == HIGH && downToggleLastReading == LOW) {
    if (((millis() - downToggleOnTime) > downToggleBounceTime) && downToggleHold != 1) {
      downToggleOnRelease();
    }
    if (downToggleHold == 1) {
      Serial.println("down held");
      digitalWrite(led, LEDstate);
      upToggleHold = 0;
    }
  }
  downToggleLastReading = reading;

  if (downToggleSingle == 1 && (millis() - downToggleLastSwitchTime) > downToggleDoubleTime) {
    Serial.println("down single press");
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
    toggleLED();
    Serial.println("down double press");
    downToggleSingle = 0;
    downToggleLastSwitchTime = millis();
  }
}

void toggleLED() {
  if (LEDstate == 0) {
    LEDstate = 1;
  } else {
    LEDstate = 0;
  }
  digitalWrite(led, LEDstate);
}

void invertLED() {
  if (LEDstate == 0) {
    digitalWrite(led, 1);
  } else {
    digitalWrite(led, 0);
  }
}