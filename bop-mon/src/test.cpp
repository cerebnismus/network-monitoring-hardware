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

    // create ICMP header
    struct icmphdr icmp_hdr;
    memset(&icmp_hdr, 0, sizeof(icmp_hdr));
    icmp_hdr.type = ICMP_ECHO;
    icmp_hdr.un.echo.id = 1234; // arbitrary id

    // for each IP in the list
    for (auto& ipStr : ipList) {
        // create destination address structure
        struct sockaddr_in dest;
        memset(&dest, 0, sizeof(dest));
        dest.sin_family = AF_INET;
        dest.sin_addr.s_addr = inet_addr(ipStr.c_str());

        // send packet
        if (sendto(sockfd, &icmp_hdr, sizeof(icmp_hdr), 0, (struct sockaddr*)&dest, sizeof(dest)) <= 0)
            std::cerr << "Failed to send packet to " << ipStr << '\n';
    }

    close(sockfd);
}