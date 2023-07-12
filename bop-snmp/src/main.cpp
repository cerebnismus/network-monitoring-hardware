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

int main(int argc, char *argv[]) {

    pb.loadIPs("iplist.txt");

    for (auto& ipStr : pb.ipList) {
        pb.bendPackets(ipStr);
    }

    return 0;
}
