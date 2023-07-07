/*
  This is example.
  Created by Oguzhan Ince (cerebnismus), July 07, 2023.

arduino-cli board list

Compile Parameters:
arduino-cli compile  \
  --fqbn arduino:avr:uno  \
  --port /dev/ttyUSB1  \
  --libraries /home/pi/bowl-of-petunias/bop-gsm/libs/  \
  --build-cache-path /home/pi/bowl-of-petunias/bop-gsm/build-cache/  \
  --export-binaries --warnings all  \
  --output-dir /home/pi/bowl-of-petunias/bop-gsm/bin/  \
  --upload  \
  --verify  \
  --verbose  \
  --clean \
  /home/pi/bowl-of-petunias/bop-gsm/bop-eth.ino

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

}


void loop() {

}