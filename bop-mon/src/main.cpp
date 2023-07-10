/**
 * @file main.cpp
 * @description Main Function
 * @author oguzhan.ince@protonmail.com
 */

#include "test.hpp"

int main() {
    PacketSender ps;
    ps.loadIPs("iplist.txt");
    ps.sendPackets();
    return 0;
}