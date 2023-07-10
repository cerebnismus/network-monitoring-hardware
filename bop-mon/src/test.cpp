/**
 * @file test.cpp
 * @description Helper Functions
 * @author oguzhan.ince@protonmail.com
 */

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <netinet/if_ether.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>

#include "test.hpp"

PacketSender::PacketSender() {}
PacketSender::~PacketSender() {}

// loads IPs from a file into a vector of strings (ipList)
void PacketSender::loadIPs(const std::string& filename) {
    std::ifstream file(filename);
    std::string line;
    while (std::getline(file, line)) {
        ipList.push_back(line);
    }
    file.close();
}

// dumps raw memory in hex byte and printable split format
void dump(const unsigned char *data_buffer, const unsigned int length) {
  unsigned char byte;
  unsigned int i, j;
  for(i=0; i < length; i++) {
    byte = data_buffer[i];
    fprintf(stdout, "%02x ", data_buffer[i]);  // display byte in hex
    if(((i%16)==15) || (i==length-1)) {
      for(j=0; j < 15-(i%16); j++)
        fprintf(stdout, "   ");
      fprintf(stdout, "| ");
      for(j=(i-(i%16)); j <= i; j++) {  // display printable bytes from line
        byte = data_buffer[j];
        if((byte > 31) && (byte < 127)) // outside printable char range
          fprintf(stdout, "%c", byte);
        else
          fprintf(stdout, ".");
      }
      fprintf(stdout, "\n"); // end of the dump line (each line 16 bytes)
  	} // end if
  } // end for
}

// Function to calculate the CRC32 checksum
unsigned int crc32(const unsigned char* buf, size_t size) {
    unsigned int crc = 0xFFFFFFFF;
    const unsigned int polynomial = 0xEDB88320;
    for (size_t i = 0; i < size; ++i) {
        crc ^= buf[i];
        for (int j = 0; j < 8; ++j) {
            if (crc & 1)
                crc = (crc >> 1) ^ polynomial;
            else
                crc >>= 1;
        }
    }
    return crc ^ 0xFFFFFFFF;
}

// generic checksum calculation algorithm
unsigned short csum(unsigned short *addr, int len) {
  int nleft = len;
  int sum = 0;
  unsigned short *w = addr;
  unsigned short answer = 0;
  while (nleft > 1) {
    sum += *w++;
    nleft -= 2;
  }
  if (nleft == 1) {
    *(unsigned char *)(&answer) = *(unsigned char *)w;
    sum += answer;
  }
  sum = (sum >> 16) + (sum & 0xffff);
  sum += (sum >> 16);
  answer = ~sum;
  return (answer);
}

void PacketSender::sendPackets() {
    std::cout << "PacketSender::sendPackets() is summoned.\n";  // debug message
    // int sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_RAW);
    int sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
    if (sockfd < 0) {
        std::cerr << "Failed to create socket. Root permissions needed.\n";
        return;
    }
    std::cout << "Socket created.\n" << std::endl;  // debug message

    // for each IP in the list
    for (auto& ipStr : ipList) {

        /* Ethernet header */
        struct ether_addr source_mac;  // Create source ethernet mac address
        memset(&source_mac, 0, sizeof(source_mac));
        source_mac.ether_addr_octet[0] = 0xDE;
        source_mac.ether_addr_octet[1] = 0xAD;
        source_mac.ether_addr_octet[2] = 0xBE;
        source_mac.ether_addr_octet[3] = 0xEF;
        source_mac.ether_addr_octet[4] = 0xCA;
        source_mac.ether_addr_octet[5] = 0xFE;

        struct ether_addr destination_mac;  // Create destination ethernet mac address
        memset(&destination_mac, 0, sizeof(destination_mac));
        destination_mac.ether_addr_octet[0] = 0xAC;
        destination_mac.ether_addr_octet[1] = 0xBC;
        destination_mac.ether_addr_octet[2] = 0x32;
        destination_mac.ether_addr_octet[3] = 0xDE;
        destination_mac.ether_addr_octet[4] = 0xAD;
        destination_mac.ether_addr_octet[5] = 0x05;

        struct ether_header eth_hdr;           // Create ethernet header structure
        memset(&eth_hdr, 0, sizeof(eth_hdr));  // Zero out the header structure
        memcpy(&eth_hdr.ether_dhost, &destination_mac, sizeof(destination_mac));
        memcpy(&eth_hdr.ether_shost, &source_mac, sizeof(source_mac));
        eth_hdr.ether_type = htons(ETHERTYPE_IP);
        // Header type id field (ETH_P_IP for IPv4) 0x0800 
        // (htons converts a short integer host byte order to network byte order)

        /* IP header */
        struct sockaddr_in destination_ip;                          // Create destination ip address structure
        memset(&destination_ip, 0, sizeof(destination_ip));         // Zero out the structure
        destination_ip.sin_family = AF_INET;                        // IPv4 address family
        destination_ip.sin_addr.s_addr = inet_addr(ipStr.c_str());  // from vector of strings

        struct sockaddr_in source_ip;              // Create source ip address structure
        memset(&source_ip, 0, sizeof(source_ip));  // Zero out the structure
        source_ip.sin_family = AF_INET;            // IPv4 address family
        source_ip.sin_addr.s_addr = inet_addr("10.28.28.14"); // manually set source ip address (for now)

        // getsockname(sockfd, (struct sockaddr*)&source_ip, &src_len); // it's getting 0.0.0.0
        // std::cout << "Source IP: " << inet_ntoa(source_ip.sin_addr) << std::endl;

        struct iphdr ip_hdr;                    // Create ip header structure
        memset(&ip_hdr, 0, sizeof(ip_hdr));     // zero out the struct
        ip_hdr.ihl = 5;                         // IP header length
        ip_hdr.version = 4;                     // IPv4
        ip_hdr.tos = 0;                         // low delay
        ip_hdr.tot_len = sizeof(ip_hdr) + sizeof(struct icmphdr);
        ip_hdr.id = htons(54321);               // the id is given by you
        ip_hdr.frag_off = 0;                    // no fragment
        ip_hdr.ttl = 255;                       // max time to live
        ip_hdr.protocol = IPPROTO_RAW;          // RAW protocol number 255 
        // ip_hdr.protocol = IPPROTO_ICMP;      // ICMP protocol number 1
        ip_hdr.check = 0;                       // set to 0 before calculating checksum
        ip_hdr.saddr = source_ip.sin_addr.s_addr;       // use source address
        ip_hdr.daddr = destination_ip.sin_addr.s_addr;  // use destination address
        // ip options are not needed right now (maybe later)

        /* ICMP header */
        struct icmphdr icmp_hdr;
        memset(&icmp_hdr, 0, sizeof(icmp_hdr));  // zero out the struct
        icmp_hdr.type = ICMP_ECHO;               // ICMP echo request number 8
        icmp_hdr.code = 0;                       // no code for echo request
        icmp_hdr.checksum = 0;                   // set to 0 before calculating checksum
        icmp_hdr.un.echo.id = 1234;              // identifier for echo request
        icmp_hdr.un.echo.sequence = 1;           // sequence number for echo request
        // icmp data is not needed right now (maybe later)

        /* Pre-calculate ip checksum */
        // Create ip packet buffer for cksum calculation
        size_t ip_packet_size = sizeof(eth_hdr) + sizeof(ip_hdr);
        char ip_packet[ip_packet_size];
        memset(ip_packet, 0, ip_packet_size);     // zero out the packet buffer

        // Copy headers to packet buffer 
        memcpy(ip_packet, &eth_hdr, sizeof(eth_hdr));
        memcpy(ip_packet + sizeof(eth_hdr), &ip_hdr, sizeof(ip_hdr));

        // Calculate checksums for IP header (checksum is calculated over the header and data)
        ip_hdr.check = csum((unsigned short*)&ip_hdr, sizeof(ip_hdr));


        /* Pre-calculate icmp checksum */
        // Create packet buffer (use total size of all headers  + data)
        size_t icmp_packet_size = sizeof(eth_hdr) + sizeof(ip_hdr) + sizeof(icmp_hdr);
        char icmp_packet[icmp_packet_size];
        memset(icmp_packet, 0, icmp_packet_size);     // zero out the packet buffer

        memcpy(icmp_packet, &eth_hdr, sizeof(eth_hdr));
        memcpy(icmp_packet + sizeof(eth_hdr), &ip_hdr, sizeof(ip_hdr));
        memcpy(icmp_packet + sizeof(eth_hdr) + sizeof(ip_hdr), &icmp_hdr, sizeof(icmp_hdr));

        // Calculate checksums for ICMP header (checksum is calculated over the header and data)
        icmp_hdr.checksum = csum((unsigned short*)&icmp_hdr, sizeof(icmp_hdr));


        // Send packet
        if (sendto(sockfd, icmp_packet, icmp_packet_size, 0, (struct sockaddr*)&destination_ip, sizeof(destination_ip)) < 0)
            std::cerr << " - Failed - " << '\n';
        else
            std::cout << " - Successed - " << '\n';

        std::cout << "Source IP: " << inet_ntoa(source_ip.sin_addr) << std::endl;
        std::cout << "Destination IP: " << inet_ntoa(destination_ip.sin_addr) << '\n' << std::endl;
        sleep(1);  // Wait 1 second before sending next packet

    }
    close(sockfd);  // Close socket file descriptor when done sending packets to all IPs
}