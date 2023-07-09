/*
 * Copyright (c) 2023 Oguzhan Ince
 * Arduino RAW Ethernet Driver
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this
 * software and associated documentation files (the "Software"), to deal in the Software
 * without restriction, including without limitation the rights to use, copy, modify,
 * merge, publish, distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to the following
 * conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
 * PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
 * OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */



// In this case, you are using raw socket and Can you use any simultaneous 
// connections, I don't know yet. But if you are using W5X00 chips, then 
// configure the maximum number of sockets to support.  W5100 chips can have
// up to 4 sockets.  W5200 & W5500 can have up to 8 sockets.  Several bytes
// of RAM are used for each socket.  Reducing the maximum can save RAM, but
// you are limited to fewer simultaneous connections.

// By default, each socket uses 2K buffers inside the Wiznet chip.  If
// MAX_SOCK_NUM is set to fewer than the chip's maximum, uncommenting
// this will use larger buffers within the Wiznet chip.  Large buffers
// can really help with UDP protocols like Artnet.  In theory larger
// buffers should allow faster TCP over high-latency links, but this
// does not always seem to work in practice (maybe Wiznet bugs?)

// Now, 8bit buffer for socket connection and not configured yet.


/*
 * Straight Ethernet Cable 
 *   PinOut Diagram
 *
 * PIN 2. White/Orange 
 * PIN 3. Orange
 * PIN 4. White/Green
 * PIN 5. Blue
 * PIN 6. White/Blue
 * PIN 7. Green
 * PIN 8. White/Brown
 * PIN 9. Brown
 */

#include <Arduino.h>

#define ETHERNET_WHITE_ORANGE  2  // White/Orange
#define ETHERNET_ORANGE        3  // Orange
#define ETHERNET_WHITE_GREEN   4  // White/Green
#define ETHERNET_BLUE          5  // Blue
#define ETHERNET_WHITE_BLUE    6  // White/Blue
#define ETHERNET_GREEN         7  // Green
#define ETHERNET_WHITE_BROWN   8  // White/Brown
#define ETHERNET_BROWN         9  // Brown

void setup() {
  pinMode(ETHERNET_WHITE_ORANGE, INPUT);
  pinMode(ETHERNET_ORANGE, INPUT);
  pinMode(ETHERNET_WHITE_GREEN, INPUT);
  pinMode(ETHERNET_BLUE, INPUT);
  pinMode(ETHERNET_WHITE_BLUE, INPUT);
  pinMode(ETHERNET_GREEN, INPUT);
  pinMode(ETHERNET_WHITE_BROWN, INPUT);
  pinMode(ETHERNET_BROWN, INPUT);
  Serial.begin(9600);
}

void loop() {
  bool v0 = digitalRead(ETHERNET_WHITE_ORANGE);
  bool v1 = digitalRead(ETHERNET_ORANGE);
  bool v2 = digitalRead(ETHERNET_WHITE_GREEN);
  bool v3 = digitalRead(ETHERNET_BLUE);
  bool v4 = digitalRead(ETHERNET_WHITE_BLUE);
  bool v5 = digitalRead(ETHERNET_GREEN);
  bool v6 = digitalRead(ETHERNET_WHITE_BROWN);
  bool v7 = digitalRead(ETHERNET_BROWN);
  Serial.println(v0, v1, v2, v3, v4, v5, v6, v7);
  // to be continue after voltage measures 

/*
Ethernet (10BASE-T and 100BASE-TX) uses differential signaling, 
with voltages that can be around +/- 2.5V. The data lines for 
Ethernet are also transformer-coupled, so the DC voltage can be 
even higher. On the other hand, the absolute maximum voltage 
for a pin on most Arduino boards is 5V. Applying voltages above 
this level may cause irreversible damage to the board.
*/

}