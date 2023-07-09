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

arduino-cli compile  \
  --fqbn arduino:avr:uno  \
  --port /dev/cu.usbserial-14120  \
  --libraries /Users/macbook/Documents/bowl-of-petunias/bop-eth/libs/  \
  --build-cache-path /Users/macbook/Documents/bowl-of-petunias/bop-eth/build-cache/  \
  --export-binaries --warnings all  \
  --output-dir /Users/macbook/Documents/bowl-of-petunias/bop-eth/bin/  \
  --upload  \
  --verify  \
  --verbose  \
  --clean \
  /Users/macbook/Documents/bowl-of-petunias/bop-eth/bop-eth.ino

*/


#include <SPI.h>
#include <Ethernet.h>
#include <SoftwareSerial.h>

// MAC addresses must be unique on the LAN and can be assigned by the user or generated here randomly.
byte destinationMAC[] = { 0x74, 0xD2, 0x1D, 0xF3, 0xAE, 0xC7 }; // Replace with your Router's MAC address
byte sourceMAC[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };      // Replace with your Arduino's MAC address

// IP addresses are dependent on your local network.
// IPAddress destinationIP(8, 8, 8, 8);  // Replace with the IP address of your destination node
IPAddress destinationIP(192, 168, 8, 106);  // Replace with the IP address of your destination node
IPAddress sourceIP(192, 168, 8, 125);       // Replace with your Arduino's IP address
IPAddress dns(192, 168, 8, 1);              // Replace with your network's DNS address
IPAddress gateway(192, 168, 8, 1);          // Replace with your Router's IP address
IPAddress subnet(255, 255, 255, 0);         // Replace with your network's subnet mask

EthernetClient ethClient;
unsigned int sequenceNumber = 0;
unsigned int ethernetInitVal = 0;


void setup() {

  ethernetInitVal = Ethernet.begin(sourceMAC);
  Serial.begin(9600);
  delay(1000); // Wait a second before continuing

  // Initialize Ethernet, returns 0 if the DHCP configuration failed, and 1 if it succeeded
  if (ethernetInitVal == 0) {
    Serial.println("Failed to configure Ethernet using DHCP");
    // Manaul configuration set a static IP address if DHCP fails to configure
    // static void begin(uint8_t *mac, IPAddress ip, IPAddress dns, IPAddress gateway, IPAddress subnet);
    Ethernet.begin(sourceMAC, sourceIP, dns, gateway, subnet);
    delay(1000); // Wait a second before continuing
    if (ethernetInitVal == 0) {
      Serial.println("Failed to configure Ethernet using static IP");
      break;
    }
  }
  delay(1000); // Wait a second before continuing
  Serial.println("Ethernet connected");
}


void loop() {

  echoRequestReply();
  delay(10000); // Wait a second before continuing

/*
  if (Serial.available()) {
    char input = Serial.read();
    if (input == 'p') {
      sendPingRequest();
      delay(1000); // Wait
    }
  }
*/

}


void echoRequestReply() {

  byte packetBuffer[48];      // Create an Ethernet packet buffer - send
  byte packetBufferMax[2046]; // Create an Ethernet packet buffer - recv

  // ETHERNET HEADER
  memcpy(packetBuffer, destinationMAC, 6);  // Destination MAC address
  memcpy(packetBuffer + 6, sourceMAC, 6);   // Source MAC address
  packetBuffer[12] = 0x08; // EtherType: IPv4 (0x0800) (0b00001000) (8) (IP packet)

  // IP HEADER
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
  memcpy(packetBuffer + 26, sourceIP.operator[](0), 4);      // Source IP address
  memcpy(packetBuffer + 30, destinationIP.operator[](0), 4); // Destination IP address

  // ICMP HEADER
  packetBuffer[34] = 0x08; // Type: ICMP Echo Request (8) (0x08)
  packetBuffer[35] = 0x00; // Code: 0 (0x00) is default for ICMP Echo Request (ping)
  packetBuffer[36] = 0x00; // Checksum (placeholder)          +
  packetBuffer[37] = 0x00; // Checksum (placeholder)          +
  packetBuffer[38] = 0x00; // Identifier (placeholder)        -
  packetBuffer[39] = 0x00; // Identifier (placeholder)        -
  packetBuffer[40] = 0x00; // Sequence Number (placeholder)   -
  packetBuffer[41] = 0x00; // Sequence Number (placeholder)   -
  
  // ICMP DATA (optional) - 32 bytes total (8 x 32 bits)
  packetBuffer[42] = 0x41; // A
  packetBuffer[43] = 0x6C; // l
  packetBuffer[44] = 0x6F; // o
  packetBuffer[45] = 0x68; // h
  packetBuffer[46] = 0x61; // a
  packetBuffer[47] = 0x21; // !

  // Calculate IP header checksum
  uint16_t ipChecksum = calculateChecksum(packetBuffer + 14, 20);
  packetBuffer[24] = ipChecksum >> 8;   // Header Checksum (high byte)
  packetBuffer[25] = ipChecksum & 0xFF; // Header Checksum (low byte)

  // Calculate ICMP header checksum
  uint16_t icmpChecksum = calculateChecksum(packetBuffer + 34, 8);
  packetBuffer[36] = icmpChecksum >> 8;   // Checksum (high byte)
  packetBuffer[37] = icmpChecksum & 0xFF; // Checksum (low byte)

  // Open a socket raw
  uint16_t socketRaw = ethClient.socketRawBegin();
  Serial.printf("sock:%d\n", socketRaw)
  

  // Calculate lengt of packetBuffer with null terminator '\0' < 2048
  // Number of bytes written, which is always equal to the size of the packet
  uint16_t packetBufferLenNull = strlen(packetBuffer) + 1;
  uint16_t packetBufferMaxLenNull = strlen(packetBuffer) + 1;

  // Send the ICMP Echo Request packet 
  // TODO: Customize socketSendAvailable for multiplexing
  ethClient.socketSend(socketRaw, packetBuffer, packetBufferLenNull);
  Serial.printf("ICMP Echo Request packet sent.\n");


  // Wait for the response
  unsigned long startTime = millis();
  while (!ethClient.socketRecvAvailable(socketRaw)) {
    if (millis() - startTime > 1000) {
      Serial.printf("Timeout: No response received.\n");
      // Immediately close socket.  If a TCP connection is established, the
      // remote host is left unaware we closed.
      // ethClient.socketClose(socketRaw);
      SPI.beginTransaction(SPI_ETHERNET_SETTINGS);
      W5100.execCmdSn(socketRaw, Sock_CLOSE);
      SPI.endTransaction();
      return;
    }
  }

  // Receive the response data
  // Returns size, or -1 for no data, or 0 if connection closed
  uint16_t responseLength = ethClient.socketRecv(socketRaw, packetBufferMax, packetBufferMaxLenNull);
  if (responseLength == 0) {
    Serial.printf("Connection closed.");
  }
  else if (responseLength == -1) {
    Serial.printf("No response received.");
  }
  else {
    Serial.printf("Received response length:%d", responseLength);
  }

  ethClient.socketClose(socketRaw);

  // TODO: Parse the response at the IP and ICMP levels
  // TODO: Print the response details


  sequenceNumber++; // Increment the sequence number for the next packet
  delay(1000); // Wait 1 second before sending the next packet

}


uint16_t calculateChecksum(const byte* data, size_t length) {
  uint32_t sum = 0;
  uint16_t* ptr = (uint16_t*)data;
  while (length > 1) {
    sum += *ptr++;
    length -= 2;
  }
  if (length == 1) {sum += *(uint8_t*)ptr;}
  sum = (sum >> 16) + (sum & 0xFFFF);
  sum += (sum >> 16);
  return ~sum;
}