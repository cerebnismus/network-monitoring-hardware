#include <Arduino.h>
#line 1 "/home/pi/bowl-of-petunias/bop-gsm/bop-gsm.ino"
/*
  btn_led_test.ino - This is example of testing button and led.
  Created by Yasin Kaya (selengalp), September 11, 2018.
*/

#include "Sixfab_Tracker.h"

SixfabTracker node;

// setup
void setup() {
  
  node.init();
  
  node.turnOnUserLED();
  delay(TIMEOUT);
  node.turnOffUserLED();
  delay(TIMEOUT);
  node.turnOnUserLED();
  delay(TIMEOUT);
  node.turnOffUserLED();
  delay(TIMEOUT);
  node.turnOnUserLED();
}

// loop
void loop() {

}
