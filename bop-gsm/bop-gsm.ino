/*
  This is example.
  Created by Yasin Kaya (selengalp), September 11, 2018.
  Updated by Oguzhan Ince (cerebnismus), July 07, 2023.


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


  tracker.sendSMS("+905304543426", "BOP2!");
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