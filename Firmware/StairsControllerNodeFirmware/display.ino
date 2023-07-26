// Display
//
//


////
// Runtime Variables
////
// Indicator Light
int indicatorState = LOW;           // ledState used to set the LED
unsigned long indicatorMillis = 0;  // will store last time LED was updated
unsigned long indicatorPreviousMillis = 0;  // will store last time LED was updated
const long flashInterval = 1000;    // interval at which to blink (milliseconds)

////
// Sub Loop
////
void handleDisplay() {
  display.clearDisplay();

  mainDisplay();
  overrideMenuDisplay();
  emergencyStopDisplay();

  display.display();
}

////
// Main Display Layout
////
void mainDisplay(void) {
  if (!manualOverride && !emergencyStopDisplay) {
    display.clearDisplay();

    display.setTextSize(2);   // Normal 1:1 pixel scale
    display.setCursor(0, 0);  // Start at top-left corner

    switch (state) {
      case 0:  // Stairs down, cable retracted
        digitalWrite(indicatorLightPin, HIGH);
        display.setTextColor(SSD1306_WHITE);  // Draw white text
        display.print(F("DOWN     "));
        display.write(47);
        display.setTextColor(SSD1306_BLACK, SSD1306_WHITE);  // Draw 'inverse' text
        display.println(F("Retracted "));
        break;
      case 1:  // Stairs raised, locking bar extended
        digitalWrite(indicatorLightPin, HIGH);
        display.setTextColor(SSD1306_WHITE);  // Draw white text
        display.print(F("RAISED   "));
        display.write(170);
        display.setTextColor(SSD1306_BLACK, SSD1306_WHITE);  // Draw 'inverse' text
        display.println(F("Locked    "));
        break;
      case 2:  // Lowering Stairs
        flashIndicator();
        display.setTextColor(SSD1306_WHITE);  // Draw white text
        display.print(F("LOWERING "));
        display.write(31);
        display.setTextColor(SSD1306_BLACK, SSD1306_WHITE);  // Draw 'inverse' text
        display.println(F("IN MOTION "));
        break;
      case 3:  // Raising Stairs
        flashIndicator();
        display.setTextColor(SSD1306_WHITE);  // Draw white text
        display.print(F("RAISING  "));
        display.write(30);
        display.setTextColor(SSD1306_BLACK, SSD1306_WHITE);  // Draw 'inverse' text
        display.println(F("IN MOTION "));
        break;
      case 4:  // Locking Bar extending
        flashIndicator();
        display.setTextColor(SSD1306_WHITE);  // Draw white text
        display.print(F("LOCKING  "));
        display.write(26);
        display.setTextColor(SSD1306_BLACK, SSD1306_WHITE);  // Draw 'inverse' text
        display.println(F("In Motion "));
        break;
      case 5:  // Locking Bar retracting
        flashIndicator();
        display.setTextColor(SSD1306_WHITE);  // Draw white text
        display.print(F("UNLOCKING"));
        display.write(27);
        display.setTextColor(SSD1306_BLACK, SSD1306_WHITE);  // Draw 'inverse' text
        display.println(F("In Motion "));
        break;
      case 6:  // Waiting for cable disconnection
        flashIndicator();
        display.setTextColor(SSD1306_WHITE);  // Draw white text
        display.println(F("UNHOOK CBL"));
        display.setTextColor(SSD1306_BLACK, SSD1306_WHITE);  // Draw 'inverse' text
        display.println(F("PRESS BTN "));
        break;
      case 7:  // Cable retracting
        flashIndicator();
        display.setTextColor(SSD1306_WHITE);  // Draw white text
        display.print(F("CABLE    "));
        display.write(24);
        display.setTextColor(SSD1306_BLACK, SSD1306_WHITE);  // Draw 'inverse' text
        display.println(F("Retracting"));
        break;
      case 8:  // Cable extending
        flashIndicator();
        display.setTextColor(SSD1306_WHITE);  // Draw white text
        display.print(F("CABLE    "));
        display.write(25);
        display.setTextColor(SSD1306_BLACK, SSD1306_WHITE);  // Draw 'inverse' text
        display.println(F("Extending "));
        break;
      case 9:  // Lowering stairs back onto locking bar
        flashIndicator();
        display.setTextColor(SSD1306_WHITE);  // Draw white text
        display.println(F("PARKING"));
        display.setTextColor(SSD1306_BLACK, SSD1306_WHITE);  // Draw 'inverse' text
        display.println(F("In Motion "));
        break;
      case 10:  // Enter manual movement mode
        flashIndicator();
        display.setTextColor(SSD1306_WHITE);  // Draw white text
        display.println(F("MANUAL    "));
        display.setTextColor(SSD1306_BLACK, SSD1306_WHITE);  // Draw 'inverse' text
        display.println(F("OVERRIDE  "));
        break;
      case 11:  // Up Toggle Held
        digitalWrite(indicatorLightPin, HIGH);
        display.setCursor(0, 0);  // Start at top-left corner
        display.print(F("HOLD UP  "));
        display.write(24);
        display.println(F("TO CONFIRM"));
        break;
      case 12:  // Down Toggle Held
        digitalWrite(indicatorLightPin, HIGH);
        display.setCursor(0, 0);  // Start at top-left corner
        display.print(F("HOLD DWN "));
        display.write(25);
        display.println(F("TO CONFIRM"));
        break;
      case 20:  // Emergency Stop
        digitalWrite(indicatorLightPin, LOW);
        display.setTextSize(2);                              // Normal 1:1 pixel scale
        display.setCursor(0, 0);                             // Start at top-left corner
        display.setTextColor(SSD1306_BLACK, SSD1306_WHITE);  // Draw 'inverse' text
        display.println(F("EMERGENCY "));
        display.println(F("  STOP!   "));
        break;
      case 30:  // Machine position lost, homing required
        digitalWrite(indicatorLightPin, LOW);
        display.setTextColor(SSD1306_BLACK, SSD1306_WHITE);  // Draw 'inverse' text
        display.println(F("HOMING    "));
        display.println(F("REQUIRED  "));
        break;
      case 31:  // Motion systems homing
        flashIndicator();
        display.setTextColor(SSD1306_BLACK, SSD1306_WHITE);  // Draw 'inverse' text
        display.println(F("HOMING    "));
        display.println(F("In Motion "));
        break;
      default:
        digitalWrite(indicatorLightPin, HIGH);
        display.setTextColor(SSD1306_WHITE);  // Draw white text
        display.println(F("UNKNOWN"));
        display.setTextColor(SSD1306_BLACK, SSD1306_WHITE);  // Draw 'inverse' text
        display.println(F("Position "));
        break;
    }
  }
}


////
// Emergency Stop Display Layout
////
void emergencyStopDisplay() {
  if (emergencyStop) {
    digitalWrite(indicatorLightPin, LOW);
    display.setTextSize(2);                              // Normal 1:1 pixel scale
    display.setCursor(0, 0);                             // Start at top-left corner
    display.setTextColor(SSD1306_BLACK, SSD1306_WHITE);  // Draw 'inverse' text
    display.println(F("EMERGENCY"));
    display.println(F("STOP!"));
  } else {
    digitalWrite(indicatorLightPin, HIGH);
  }
}


////
// Manual Movement Override Display Layout
////
void updateOverrideMenu() {
  overrideBar = !overrideBar;
}

void overrideMenuDisplay() {
  if (manualOverride) {
    switch (manualMenuState) {
      case 4:  // Stairs down, cable retracted
        digitalWrite(indicatorLightPin, LOW);
        display.setTextColor(SSD1306_BLACK, SSD1306_WHITE);  // Draw 'inverse' text
        display.println(F("Hoist Cable"));
        display.println(F("Manual CTL"));
        break;
      case 7:  // Stairs raised, locking bar extended
        digitalWrite(indicatorLightPin, LOW);
        display.setTextColor(SSD1306_BLACK, SSD1306_WHITE);  // Draw 'inverse' text
        display.println(F("Lock Bar  "));
        display.println(F("Manual CTL"));
        break;
    }
  }
}


////
// Flash Indicator Light
////
void flashIndicator() {
  unsigned long indicatorMillis = millis();

  if (indicatorMillis - indicatorPreviousMillis >= flashInterval) {
    // save the last time you blinked the LED
    indicatorPreviousMillis = indicatorMillis;

    // if the LED is off turn it on and vice-versa:
    if (indicatorState == LOW) {
      indicatorState = HIGH;
    } else {
      indicatorState = LOW;
    }

    // set the LED with the indicatorState of the variable:
    digitalWrite(indicatorLightPin, indicatorState);
  }
}


////
// Boot Screen Layout
////
void bootScreenDisplay(void) {
  display.clearDisplay();

  display.setTextSize(1);               // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE);  // Draw white text
  display.setCursor(0, 0);              // Start at top-left corner
  display.println(F("Stairs Controller"));
  display.println(F("Control Node FW.V1"));

  display.setTextColor(SSD1306_BLACK, SSD1306_WHITE);  // Draw 'inverse' text
  display.println(F("Booting..."));

  display.display();
  delay(2000);
}
