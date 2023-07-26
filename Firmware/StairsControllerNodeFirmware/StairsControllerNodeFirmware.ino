// Stairs Controller Node Firmware v1
// Chris Loidolt
// 102/21/2023


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
// On an arduino UNO:       A4(SDA), A5(SCL)
// On an arduino MEGA 2560: 20(SDA), 21(SCL)
#define OLED_RESET -1        // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C  ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);


////
// Serial Communication
////
#include <SoftwareSerial.h>
// Software Serial Pins
const byte rxPin = 10;
const byte txPin = 11;
SoftwareSerial Controller(rxPin, txPin);


////
// Define Pins
////
const byte eStopPin = 4;
const byte indicatorLightPin = 5;

// Input Pins
const byte arcadeBtnPin = 8;
const byte upTogglePin = 6;
const byte downTogglePin = 7;


////
// Runtime Variables
////
String state = "unk";
String terminalState = "unk";
String controllerState = "unk";

bool homed = false;
bool manualOverride = false;
bool overrideBar = false;

bool emergencyStop = false;
bool reportState = false;


////
// Initial Setup
////
void setup() {
  Serial.begin(115200);

  // Define pin modes for TX and RX
  pinMode(rxPin, INPUT);
  pinMode(txPin, OUTPUT);

  Controller.begin(115200);

  pinMode(eStopPin, INPUT_PULLUP);
  pinMode(arcadeBtnPin, INPUT_PULLUP);
  pinMode(upTogglePin, INPUT_PULLUP);
  pinMode(downTogglePin, INPUT_PULLUP);
  pinMode(indicatorLightPin, OUTPUT);

  digitalWrite(eStopPin, HIGH);
  digitalWrite(indicatorLightPin, HIGH);

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;)
      ;  // Don't proceed, loop forever
  }

  bootScreenDisplay();

  //Clear display
  display.clearDisplay();
  display.display();
  delay(200);
}


////
// Main Loop
////
void loop() {
  serial();
  input();
  handleDisplay();

  if (emergencyStop) {
    reportState = true;
  }
  if (reportState) {
    handleReportState();
  }
}
