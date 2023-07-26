#include <SoftwareSerial.h>

const byte rxPin = 2;
const byte txPin = 3;

// Set up a new SoftwareSerial object
SoftwareSerial lockingBarSerial(rxPin, txPin);

String command;

void setup() {
  Serial.begin(9600);

  // Define pin modes for TX and RX
  pinMode(rxPin, INPUT);
  pinMode(txPin, OUTPUT);

  // Set the baud rate for the SoftwareSerial object
  lockingBarSerial.begin(4800);
}

void loop() {
  handleSerial();

  if (lockingBarSerial.available() > 0) {
    /* Serial.println(mySerial.read()); */
    command = lockingBarSerial.readStringUntil('\n');
    Serial.println("lockingBarSerial: " + command);
  }
}

void handleSerial() {
  if (Serial.available() > 0) {
    command = Serial.readStringUntil('\n');

    if (command.equals("send_stop")) {
      Serial.println("Sending Stop");
      lockingBarSerial.write("stop\n");
    } else if (command.equals("send_state")) {
      Serial.println("Requesting State");
      lockingBarSerial.write("state\n");
    } else if (command.equals("send_reset")) {
      Serial.println("Send Reset Command");
      lockingBarSerial.write("reset\n");
    } else if (command.equals("send_extend")) {
      Serial.println("Send Extend Command");
      lockingBarSerial.write("extend\n");
    } else if (command.equals("send_retract")) {
      Serial.println("Send Retract Command");
      lockingBarSerial.write("retract\n");
    } else {
      Serial.println(command);
    }
  }
}