/**
 * @file test.cpp
 * @description Helper Functions
 * @author oguzhan.ince@protonmail.com
 */

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>

#include "test.hpp"

PacketSender::PacketSender() {}

PacketSender::~PacketSender() {}

void PacketSender::loadIPs(const std::string& filename) {
    std::ifstream file(filename);
    std::string line;
    while (std::getline(file, line)) {
        ipList.push_back(line);
    }
    file.close();
}

void PacketSender::sendPackets() {
    int sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
    if (sockfd < 0) {
        std::cerr << "Failed to create socket. Root permissions needed.\n";
        return;
    }

    // for each IP in the list
    for (auto& ipStr : ipList) {

        // create destination address structure
        struct sockaddr_in dest;
        memset(&dest, 0, sizeof(dest));
        dest.sin_family = AF_INET;
        dest.sin_addr.s_addr = inet_addr(ipStr.c_str());

        // get source ipv4 address from the interface
        struct sockaddr_in src;
        socklen_t src_len = sizeof(src);
        getsockname(sockfd, (struct sockaddr*)&src, &src_len);
        printf("Source IP: %s\n", inet_ntoa(src.sin_addr));


        // create IP header
        struct iphdr ip_hdr;
        memset(&ip_hdr, 0, sizeof(ip_hdr));
        ip_hdr.ihl = 5;
        ip_hdr.version = 4;
        ip_hdr.tos = 0;
        ip_hdr.tot_len = sizeof(ip_hdr) + sizeof(struct icmphdr);
        ip_hdr.id = htons(54321); // the id is given by you
        ip_hdr.frag_off = 0;
        ip_hdr.ttl = 255;
        ip_hdr.protocol = IPPROTO_ICMP;
        ip_hdr.check = 0;      // set to 0 before calculating checksum
        ip_hdr.saddr = src.sin_addr.s_addr; // use source address
        ip_hdr.daddr = dest.sin_addr.s_addr; // use destination address


        // create ICMP header
        struct icmphdr icmp_hdr;
        memset(&icmp_hdr, 0, sizeof(icmp_hdr));
        icmp_hdr.type = ICMP_ECHO;
        icmp_hdr.un.echo.id = 1234; // arbitrary id

        // send packet
        if (sendto(sockfd, &icmp_hdr, sizeof(icmp_hdr), 0, (struct sockaddr*)&dest, sizeof(dest)) <= 0)
            std::cerr << "Failed to send packet to " << ipStr << '\n';
    }

    close(sockfd);
}