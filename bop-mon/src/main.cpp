/**
 * @file main.cpp
 * @details main function
 * @copyright oguzhan.ince@protonmail.com
 */

#include "test.hpp"

int main() {
    PacketSender ps;
    ps.loadIPs("iplist.txt");
    ps.sendPackets();
    return 0;
}