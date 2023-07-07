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

#include <SPI.h>
#include <Ethernet.h>
#include <SoftwareSerial.h>
#include "/home/pi/bowl-of-petunias/bop-gsm/libs/Sixfab_Tracker.h"
#include "/home/pi/bowl-of-petunias/bop-gsm/libs/Sixfab_Tracker.cpp"


byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED }; // MAC address of your Ethernet shield
IPAddress serverIP(192, 168, 8, 1); // IP address of the remote server
EthernetClient client;

void setup() {
  Serial.begin(9600);
  
  // Initialize Ethernet
  if (Ethernet.begin(mac) == 0) {
    Serial.println("Failed to configure Ethernet using DHCP");
    // You can also set a static IP address if DHCP fails
    // Ethernet.begin(mac, IPAddress(192, 168, 8, 120));
  }

  delay(1000);
  Serial.println("Ethernet connected");
}

void loop() {
  // Make a HTTP GET request to the remote server
  if (client.connect(serverIP, 80)) {
    Serial.println("Connected to server");
    client.println("GET / HTTP/1.1");
    client.println("Host: 192.168.8.1"); // Replace with the actual hostname or IP address of the server
    client.println("Connection: close");
    client.println();
  }
  
  delay(2000); // Wait for the server to respond
  
  // Read the response from the server
  while (client.available()) {
    char c = client.read();
    Serial.print(c);
  }
  
  // Disconnect from the server
  client.stop();
  
  delay(5000); // Wait for 5 seconds before making the next request
}
