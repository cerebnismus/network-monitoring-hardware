/**
 * @file main.cpp
 * @details main function
 * @copyright oguzhan.ince@protonmail.com
 * @todo args and usage helper etc.
 */

#include <iostream>
#include <unistd.h>
#include <fstream>
#include <cstring>  // memset etc.

#include "icmp.hpp"

PacketBender pb;

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        std::cerr << "Usage: " << argv[0] << " ttl_value\n";
        return 1;
    }

    int ttl = std::stoi(argv[1]);
    PacketBender pb;

    pb.loadIPs("iplist.txt");

    for (const auto& ipStr : pb.ipList) {
        pb.bendPackets(ipStr, ttl);
    }

    return 0;
}