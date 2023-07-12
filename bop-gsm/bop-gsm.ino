/*
  NAME: Bowl of Petunias - GSM (bop-gsm)
  AUTH: Oguzhan Ince (cerebnismus)
  MAIL: <oguzhan.ince@protonmail.com>
  DATE: July 07, 2023.
  DESC: Send a icmp requests to a remote network nodes.
  
  The project is based on the Arduino SoftwareSerial library and the Sixfab_Tracker.
  Library is a mbed library that implements the SoftwareSerial for the Arduino.
  The SoftwareSerial library has been developed to allow serial communication 
  on other digital pins of the Arduino, using software to replicate the functionality 
  (hence the name "SoftwareSerial ").

  https://github.com/sixfab/Sixfab_Arduino_Tracker_Shield/
  Created by Yasin Kaya (selengalp), September 11, 2018.
  Updated by Oguzhan Ince (cerebnismus), July 07, 2023.

  Can be used only with the following Sixfab_Tracker functions:
    init(), turnOnUserLED(), turnOffUserLED(), sendSMS(), sendATCommand()
  The following functions are not supported:
    turnOnNetworkLED(), turnOffNetworkLED(), turnOnPowerLED(), turnOffPowerLED(), 
    turnOnGPRS(), turnOffGPRS(), turnOnGPS(), turnOffGPS(), turnOnGNSS(), turnOffGNSS(), 
    turnOnBluetooth(), turnOffBluetooth(), turnOnBluetoothAT(), turnOffBluetoothAT(), 

INSTALLATION: Arduino CLI Commands for this project (bop-eth):
arduino-cli board list
arduino-cli lib install "Sixfab_Tracker"
arduino-cli lib install "SoftwareSerial@1.0.0"

Compile Parameters:
arduino-cli compile  \
  --fqbn arduino:avr:uno  \
  --port /dev/ttyUSB0  \
  --libraries /home/pi/bowl-of-petunias/bop-gsm/libs/  \
  --build-cache-path /home/pi/bowl-of-petunias/bop-gsm/build-cache/  \
  --export-binaries --warnings all  \
  --output-dir /home/pi/bowl-of-petunias/bop-gsm/bin/  \
  --upload  \
  --verify  \
  --verbose  \
  --clean \
  /home/pi/bowl-of-petunias/bop-gsm/bop-gsm.ino
*/

#include <SoftwareSerial.h>
#include "/home/pi/bowl-of-petunias/bop-gsm/libs/Sixfab_Tracker.h"
#include "/home/pi/bowl-of-petunias/bop-gsm/libs/Sixfab_Tracker.cpp"

SixfabTracker tracker;

void setup() {
  
  tracker.init();
  
  tracker.turnOnUserLED();
  delay(1000);
  tracker.turnOffUserLED();
  delay(1000);
  tracker.turnOnUserLED();
  delay(1000);
  tracker.turnOffUserLED();
  delay(1000);
  tracker.turnOnUserLED();
  delay(1000);


  tracker.sendSMS("+XXXXXXXXXXXX", "BOP-GSM is online");
  DEBUG.println("SMS sent!");

  // AT command to configure GSM module for text mode SMS
  sendATCommand("AT+CMGF=1\r\n", 1000);
}


void loop() {
  if (Serial.available()) {
    char input = Serial.read();
    if (input == 'p') {
      icmpPing();
    }
  }
}


void sendATCommand(String command, int delayTime) {
  gsmSerial.print(command);
  delay(delayTime);
  while (gsmSerial.available()) {
    response = gsmSerial.readString();
    Serial.print(response);
  }
}


void icmpPing() {
  // AT command to send ICMP Echo Request to the target IP
  String command = "AT+PING=\"192.168.8.106\"\r\n";
  sendATCommand(command, 5000);
}