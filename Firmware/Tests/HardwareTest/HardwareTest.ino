////
// LCD Setup
////

// LCD Wiring
//
//-------------------
// Pin A4   SDA |*| |
// Pin A5   SCK |*| |
// +5V      VCC |*| |
// GND      GND |*| |<
//-------------------

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128  // OLED display width, in pixels
#define SCREEN_HEIGHT 32  // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
// The pins for I2C are defined by the Wire-library.
// On an arduino UNO:       A4(SDA), A5(SCL)
// On an arduino MEGA 2560: 20(SDA), 21(SCL)
// On an arduino LEONARDO:   2(SDA),  3(SCL), ...
#define OLED_RESET -1        // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C  ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define LOGO_HEIGHT 16
#define LOGO_WIDTH 16
static const unsigned char PROGMEM logo_bmp[] = { 0b00000000, 0b11000000,
                                                  0b00000001, 0b11000000,
                                                  0b00000001, 0b11000000,
                                                  0b00000011, 0b11100000,
                                                  0b11110011, 0b11100000,
                                                  0b11111110, 0b11111000,
                                                  0b01111110, 0b11111111,
                                                  0b00110011, 0b10011111,
                                                  0b00011111, 0b11111100,
                                                  0b00001101, 0b01110000,
                                                  0b00011011, 0b10100000,
                                                  0b00111111, 0b11100000,
                                                  0b00111111, 0b11110000,
                                                  0b01111100, 0b11110000,
                                                  0b01110000, 0b01110000,
                                                  0b00000000, 0b00110000 };


////
// Define Pins
////
const byte eStopPin = 4;
const byte indicatorLightPin = 5;
const byte upPin = 6;
const byte downPin = 7;
const byte arcadeBtnPin = 8;


////
// Runtime Variables
////
int programState = 0;


////
// Input Handler Variables
////

// Emergency Stop Button
unsigned long eStopDebounceDelay = 50;    // the debounce time; increase if the output flickers
int eStopState = 0;                       // the current reading from the input pin
int lastEStopState = 0;                   // the previous reading from the input pin
unsigned long lastEStopDebounceTime = 0;  // the last time the output pin was toggled

// Arcade Button
unsigned long debounceDelay = 50;
int buttonState = 0;
int lastButtonState = 0;
unsigned long lastBtnDebounceTime = 0;

//Up Toggle
int upToggleState = 0;
long upToggleMillis = 0;

//Down Toggle
int downToggleState = 0;
long downToggleMillis = 0;

const long holdInterval = 3000;


void setup() {
  delay(2000);

  Serial.begin(9600);
  Serial.println("Stairs Controller Node");

  pinMode(eStopPin, INPUT_PULLUP);
  pinMode(upPin, INPUT_PULLUP);
  pinMode(downPin, INPUT_PULLUP);
  pinMode(arcadeBtnPin, INPUT_PULLUP);

  pinMode(indicatorLightPin, OUTPUT);

  digitalWrite(eStopPin, HIGH);
  digitalWrite(upPin, HIGH);
  digitalWrite(downPin, HIGH);
  digitalWrite(arcadeBtnPin, HIGH);
  digitalWrite(indicatorLightPin, HIGH);



  //Test Relay Twice
  testRelay();
  testRelay();

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;)
      ;  // Don't proceed, loop forever
  }

  // Show initial display buffer contents on the screen --
  // the library initializes this with an Adafruit splash screen.
  display.display();
  delay(2000);  // Pause for 2 seconds

  // Clear the buffer
  display.clearDisplay();

  // Show the display buffer on the screen. You MUST call display() after
  // drawing commands to make them visible on screen!
  display.display();
  delay(2000);

  testdrawbitmap();  // Draw a small bitmap image
}

void loop() {
  arcadeButton();
  upButton();

  emergencyStop();
}

void testRelay() {
  digitalWrite(indicatorLightPin, LOW);
  delay(1000);
  digitalWrite(indicatorLightPin, HIGH);
  delay(1000);
}

void arcadeButton() {
  int reading = digitalRead(arcadeBtnPin);
  if (reading != lastButtonState) {
    lastBtnDebounceTime = millis();
  }

  if ((millis() - lastBtnDebounceTime) > debounceDelay) {
    if (reading != buttonState) {
      buttonState = reading;
      if (buttonState == 1) {
        Serial.println("Arcade Pressed");
      }
    }
  }
  lastButtonState = reading;
}

void upButton() {
  upToggleState = digitalRead(upPin);

  if (upToggleState == LOW && programState == 0) {
    upToggleMillis = millis();
    drawHoldPrompt();
    programState = 1;

  } else if (programState == 1 && upToggleState == 1) {
    programState = 0;  //reset
  }
  if (millis() - upToggleMillis > holdInterval && programState == 1) {
    programState = 2;
    Serial.println("Up Held");
    display.clearDisplay();
  }
  if (upToggleState == 1) {
    programState = 0;
    display.clearDisplay();
  }
}

void emergencyStop() {
  int reading = digitalRead(eStopPin);
  if (reading != lastEStopState) {
    lastEStopDebounceTime = millis();
  }

  if ((millis() - lastEStopDebounceTime) > eStopDebounceDelay) {
    if (reading != lastEStopState) {
      eStopState = reading;
      if (eStopState == 1) {
        Serial.println("!! Emergency Stop !!");
      }
    }
  }
  lastEStopState = reading;
}

void drawHoldPrompt(void) {
  display.clearDisplay();

  display.setTextSize(1);               // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE);  // Draw white text
  display.setCursor(0, 0);              // Start at top-left corner
  display.println(F("HOLD UP"));
  display.println(F("TO CONFIRM"));

  display.setTextColor(SSD1306_BLACK, SSD1306_WHITE);  // Draw 'inverse' text
  display.println(3.141592);

  display.display();
  delay(2000);
}

void testdrawbitmap(void) {
  display.clearDisplay();

  display.drawBitmap(
    (display.width() - LOGO_WIDTH) / 2,
    (display.height() - LOGO_HEIGHT) / 2,
    logo_bmp, LOGO_WIDTH, LOGO_HEIGHT, 1);
  display.display();
  delay(1000);
}

#define XPOS 0  // Indexes into the 'icons' array in function below
#define YPOS 1
#define DELTAY 2
