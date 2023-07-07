/*
  btn_led_test.ino - This is example of testing button and led.
  Created by Yasin Kaya (selengalp), September 11, 2018.
  arduino-cli compile --fqbn arduino:avr:uno --port /dev/ttyUSB0 --libraries /home/pi/bowl-of-petunias/bop-gsm/libs/ --build-cache-path /home/pi/bowl-of-petunias/bop-gsm/build-cache/ --export-binaries --warnings all --output-dir /home/pi/bowl-of-petunias/bop-gsm/bin/ /home/pi/bowl-of-petunias/bop-gsm/bop-gsm.ino --upload --verify --verbose --clean
*/

#include "/home/pi/bowl-of-petunias/bop-gsm/libs/Sixfab_Tracker.h"
#include "/home/pi/bowl-of-petunias/bop-gsm/libs/Sixfab_Tracker.cpp"

SixfabTracker node;

void setup() {
  
  node.init();
  
  node.turnOnUserLED();
  delay(1000);
  node.turnOffUserLED();
  delay(1000);
  node.turnOnUserLED();
  delay(1000);
  node.turnOffUserLED();
  delay(1000);
  node.turnOnUserLED();
  delay(1000);
}

// loop
void loop() {

}