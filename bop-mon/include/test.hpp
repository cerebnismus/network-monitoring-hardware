/**
 * @file test.hpp
 * @details Helper Functions
 * @author oguzhan.ince@protonmail.com
 */

#ifndef TEST_HPP
#define TEST_HPP

#include <string>
#include <vector>

class PacketSender {
public:
    PacketSender();
    ~PacketSender();

    // Loads IPs from the provided file
    void loadIPs(const std::string& filename);

    // Sends ICMP echo requests to all loaded IPs
    void sendPackets();

private:
    std::vector<std::string> ipList;
};

#endif // TEST_HPP
