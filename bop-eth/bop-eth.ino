/*
  NAME: Bowl of Petunias - Ethernet (bop-eth)
  AUTH: Oguzhan Ince (cerebnismus) 
  MAIL: <oguzhan.ince@protonmail.com>
  DATE: July 07, 2023.
  DESC: Send a icmp and snmp requests to a remote network nodes. 
  
  The project is based on the Arduino Ethernet library and the W5100Interface. 
  Library is a mbed library that implements the EthernetInterface for the
  Wiznet W5100 based Ethernet shield. The Wiznet W5100 chip supports up to
  four simultaneous socket connections. And never supports more than 4 sockets.

  https://os.mbed.com/users/hudakz/code/W5100Interface/
  Can be used only with the following EthernetInterface socket related functions:
    socket(), connect(), send(), recv(), close()
  The following functions are not supported:
    accept(), bind(), listen(), sendto(), recvfrom(), setsockopt(), getsockopt()

  The default pinout can be overridden in mbed_app.json
  The W5100Interface class uses the following pins:
    D2  - SPI MOSI
    D3  - SPI MISO
    D4  - SPI SCLK
    D10 - SPI CS
    D7  - Wiznet W5100 reset

INSTALLATION: Arduino CLI Commands for this project (bop-eth):
arduino-cli board list
arduino-cli lib install "Ethernet@2.0.0"
arduino-cli lib install "SoftwareSerial@1.0.0"

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
#include <SoftwareSerial.h>

// MAC addresses must be unique on the LAN and can be assigned by the user or generated here randomly.
byte sourceMAC[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED }; // Replace with your Arduino's MAC address
byte destinationMAC[] = { 0x74, 0xD2, 0x1D, 0xF3, 0xAE, 0xC7 }; // Replace with your Arduino's MAC address

IPAddress localIP(192, 168, 8, 125); // Replace with your Arduino's IP address
IPAddress targetIP(192, 168, 8, 106); // Replace with the IP address of the target device to ping
IPAddress remoteIP(192, 168, 8, 1); // IP address of the remote server

EthernetClient ethClient;
unsigned int sequenceNumber = 0;

void setup() {
  Ethernet.begin(mac, localIP);
  Serial.begin(9600);

  // Initialize Ethernet, returns 0 if the DHCP configuration failed, and 1 if it succeeded
  if (Ethernet.begin(mac) == 0) {
    Serial.println("Failed to configure Ethernet using DHCP");
    // Manaul configuration set a static IP address if DHCP fails to configure
    // static void begin(uint8_t *mac, IPAddress ip, IPAddress dns, IPAddress gateway, IPAddress subnet);
    Ethernet.begin(mac, IPAddress(192, 168, 8, 125), IPAddress(192, 168, 8, 1), IPAddress(255, 255, 255, 0));
  }

  delay(1000);
  Serial.println("Ethernet connected");

  // Make a HTTP GET request to the remote server
  // In this case, request to get the router's root page
  if (ethClient.connect(remoteIP, 80)) {
    Serial.println("Connected to server");
    ethClient.println("GET / HTTP/1.1");
    ethClient.println("Host: 192.168.8.1");
    ethClient.println("Connection: close");
    ethClient.println();
  }

  delay(2000); // Wait for the server to respond
               // Read the response from the server
  while (ethClient.available()) {
    char c = ethClient.read();
    Serial.print(c);
  }

  ethClient.stop(); // Disconnect from the server
  delay(1000);      // Wait a second before continuing
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
  
  byte packetBuffer[48]; // Create an Ethernet packet buffer

  // Ethernet header
  memcpy(packetBuffer, targetmac, 6); // Destination MAC address
  memcpy(packetBuffer + 6, Ethernet.MACAddress(), 6); // Source MAC address
  packetBuffer[12] = 0x08; // EtherType: IPv4

  // IP header
  // IHL: Internet Header Length (4 bits): Number of 32-bit words in the header
  // IHL = 5 (minimum value) (0b0101) (5 * 32 bits = 160 bits = 20 bytes)
  // High nibble: version, low nibble: header length in 32-bit words (5)
  packetBuffer[14] = 0x45; // Version (4), IHL (5)
  packetBuffer[15] = 0x00; // Type of Service (0) (DSCP + ECN) (0x00) (0b00000000) (best effort)
  packetBuffer[16] = 0x00; // Total Length (placeholder)      -
  packetBuffer[17] = 0x00; // Total Length (placeholder)      -
  packetBuffer[18] = 0x00; // Identification (placeholder)    -
  packetBuffer[19] = 0x00; // Identification (placeholder)    -
  packetBuffer[20] = 0x00; // Flags and Fragment Offset       ?
  packetBuffer[21] = 0x00; // Flags and Fragment Offset       ?
  packetBuffer[22] = 0x40; // TTL (64)
//packetBuffer[23] = 0x01; // Protocol: ICMP (1) (0x01) 
  packetBuffer[23] = 0xFF; // Protocol: RAW (255) (0xFF)
  packetBuffer[24] = 0x00; // Header Checksum (placeholder)   +
  packetBuffer[25] = 0x00; // Header Checksum (placeholder)   +
  memcpy(packetBuffer + 26, localIP.raw_address(), 4);  // Source IP address
  memcpy(packetBuffer + 30, targetIP.raw_address(), 4); // Destination IP address

  // ICMP header
  packetBuffer[34] = 0x08; // Type: ICMP Echo Request (8) (0x08)
  packetBuffer[35] = 0x00; // Code: 0 (0x00) is default for ICMP Echo Request (ping)
  packetBuffer[36] = 0x00; // Checksum (placeholder)          +
  packetBuffer[37] = 0x00; // Checksum (placeholder)          +
  packetBuffer[38] = 0x00; // Identifier (placeholder)        -
  packetBuffer[39] = 0x00; // Identifier (placeholder)        -
  packetBuffer[40] = 0x00; // Sequence Number (placeholder)   -
  packetBuffer[41] = 0x00; // Sequence Number (placeholder)   -
  
  // ICMP Echo Request Data (Aloha!) 
  // 32 bytes total (8 x 32 bits)
  // 0x00 is the default
  packetBuffer[42] = 0x41; // A
  packetBuffer[43] = 0x6C; // l
  packetBuffer[44] = 0x6F; // o
  packetBuffer[45] = 0x68; // h
  packetBuffer[46] = 0x61; // a
  packetBuffer[47] = 0x21; // !


  // Calculate IP header checksum
  uint16_t ipChecksum = calculateChecksum(packetBuffer + 14, 20);
  packetBuffer[24] = ipChecksum >> 8; // Header Checksum (high byte)
  packetBuffer[25] = ipChecksum & 0xFF; // Header Checksum (low byte)

  // Calculate ICMP header checksum
  uint16_t icmpChecksum = calculateChecksum(packetBuffer + 34, 8);
  packetBuffer[36] = icmpChecksum >> 8; // Checksum (high byte)
  packetBuffer[37] = icmpChecksum & 0xFF; // Checksum (low byte)

  // Send the ICMP packet
  // Ethernet.beginPacket(packetBuffer, sizeof(packetBuffer));
  // Ethernet.write(packetBuffer, sizeof(packetBuffer));
  // Ethernet.endPacket();

  // Send the ICMP Echo Request packet
  // Open a RAW connection to the target IP address
  // Returns 1 if successful, 0 if there are no sockets available to use
  if (!ethClient.connect(targetIP, 0)) {
    Serial.println("Failed to open RAW connection");
    return;
  }

  // Send the ICMP packet
  // Returns the number of bytes written, which is always equal to the size of the packet
  int bytesWritten = ethClient.write(packetBuffer, sizeof(packetBuffer));
  if (bytesWritten == 0) {
    Serial.println("Failed to write to socket");
    return;
  }

  ethClient.stop(); // Close the RAW connection
  Serial.println("ICMP Echo Request packet sent.");
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