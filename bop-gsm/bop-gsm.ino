/*
  btn_led_test.ino - This is example of testing button and led.
  Created by Yasin Kaya (selengalp), September 11, 2018.

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

  // Make sure the module is connected to the network
  if (tracker.isNetworkConnected()) {
    // Send an SMS
    tracker.sendSMS("+905304543426", "BOP!");

    // Print a message to indicate that the SMS was sent
    DEBUG.println("SMS sent successfully!");
  } else {
    // Print a message to indicate that the module is not connected to the network
    DEBUG.println("Network is not connected!");
  }
}


void loop() {

}