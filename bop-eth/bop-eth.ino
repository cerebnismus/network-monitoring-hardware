/*
  This is example.
  Created by Oguzhan Ince (cerebnismus), July 07, 2023.

arduino-cli board list
arduino-cli lib install "Ethernet@2.0.0"
arduino-cli lib install "SoftwareSerial@1.0.0"

Compile Parameters:
arduino-cli compile  \
  --fqbn arduino:avr:uno  \
  --port /dev/ttyUSB1  \
  --libraries /home/pi/bowl-of-petunias/bop-eth/libs/  \
  --build-cache-path /home/pi/bowl-of-petunias/bop-eth/build-cache/  \
  --export-binaries --warnings all  \
  --output-dir /home/pi/bowl-of-petunias/bop-eth/bin/  \
  --upload  \
  --verify  \
  --verbose  \
  --clean \
  /home/pi/bowl-of-petunias/bop-eth/bop-eth.ino
*/

#include <SPI.h>
#include <Ethernet.h>
// #include <EthernetUdp.h>
#include <SoftwareSerial.h>

byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED }; // Replace with your Arduino's MAC address
IPAddress localIP(192, 168, 8, 125); // Replace with your Arduino's IP address
IPAddress targetIP(192, 168, 8, 106); // Replace with the IP address of the target device to ping
IPAddress remoteIP(192, 168, 8, 1); // IP address of the remote server

// EthernetUDP udp;
EthernetClient ethClient;
unsigned int sequenceNumber = 0;

void setup() {
  Ethernet.begin(mac, localIP);
  Serial.begin(9600);
  udp.begin(161); // Port for UDP communication

  // Initialize Ethernet
  if (Ethernet.begin(mac) == 0) {
    Serial.println("Failed to configure Ethernet using DHCP");
    // set a static IP address if DHCP fails
    Ethernet.begin(mac, IPAddress(192, 168, 8, 125));
  }

  delay(1000);
  Serial.println("Ethernet connected");


  // Make a HTTP GET request to the remote server
  // In this case, request to get the router's root page
  if (client.connect(remoteIP, 80)) {
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
  delay(1000);

}


void loop() {
  if (Serial.available()) {
    char input = Serial.read();
    if (input == 'p') {
      sendPingRequest();
    }
  }
}

void sendPingRequest() {
  // Create an Ethernet packet buffer
  byte packetBuffer[64];

  // Ethernet header
  memcpy(packetBuffer, mac, 6); // Destination MAC address
  memcpy(packetBuffer + 6, Ethernet.macAddress(), 6); // Source MAC address
  packetBuffer[12] = 0x08; // EtherType: IPv4

  // IP header
  packetBuffer[14] = 0x45; // Version (4), IHL (5)
  packetBuffer[15] = 0x00; // Type of Service
  packetBuffer[16] = 0x00; // Total Length (placeholder)
  packetBuffer[17] = 0x00; // Total Length (placeholder)
  packetBuffer[18] = 0x00; // Identification (placeholder)
  packetBuffer[19] = 0x00; // Identification (placeholder)
  packetBuffer[20] = 0x00; // Flags and Fragment Offset
  packetBuffer[21] = 0x00; // Flags and Fragment Offset
  packetBuffer[22] = 0x40; // TTL (64)
  packetBuffer[23] = 0x01; // Protocol: ICMP (1)
  packetBuffer[24] = 0x00; // Header Checksum (placeholder)
  packetBuffer[25] = 0x00; // Header Checksum (placeholder)
  memcpy(packetBuffer + 26, localIP.raw_address(), 4); // Source IP address
  memcpy(packetBuffer + 30, targetIP.raw_address(), 4); // Destination IP address

  // ICMP header
  packetBuffer[34] = 0x08; // Type: ICMP Echo Request
  packetBuffer[35] = 0x00; // Code: 0
  packetBuffer[36] = 0x00; // Checksum (placeholder)
  packetBuffer[37] = 0x00; // Checksum (placeholder)
  packetBuffer[38] = 0x00; // Identifier (placeholder)
  packetBuffer[39] = 0x00; // Identifier (placeholder)
  packetBuffer[40] = 0x00; // Sequence Number (placeholder)
  packetBuffer[41] = 0x00; // Sequence Number (placeholder)

  // Calculate IP header checksum
  uint16_t ipChecksum = calculateChecksum(packetBuffer + 14, 20);
  packetBuffer[24] = ipChecksum >> 8; // Header Checksum (high byte)
  packetBuffer[25] = ipChecksum & 0xFF; // Header Checksum (low byte)

  // Calculate ICMP header checksum
  uint16_t icmpChecksum = calculateChecksum(packetBuffer + 34, 8);
  packetBuffer[36] = icmpChecksum >> 8; // Checksum (high byte)
  packetBuffer[37] = icmpChecksum & 0xFF; // Checksum (low byte)

  // Send the ICMP packet
  Ethernet.beginPacket(packetBuffer, sizeof(packetBuffer));
  Ethernet.write(packetBuffer, sizeof(packetBuffer));
  Ethernet.endPacket();

  Serial.println("Ping sent.");
}

uint16_t calculateChecksum(const byte* data, size_t length) {
  uint32_t sum = 0;
  uint16_t* ptr = (uint16_t*)data;

  while (length > 1) {
    sum += *ptr++;
    length -= 2;
  }

  if (length == 1) {
    sum += *(uint8_t*)ptr;
  }

  sum = (sum >> 16) + (sum & 0xFFFF);
  sum += (sum >> 16);
  
  return ~sum;
}