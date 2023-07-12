/**
 * @file trace.hpp
 * @details Helper Functions
 * @author oguzhan.ince@protonmail.com
 */

#ifndef TRACE_HPP
#define TRACE_HPP

#include <string>
#include <vector>

class PacketBender {
public:
    PacketBender();
    ~PacketBender();

    std::vector<std::string> ipList;

    // Loads IPs from a file into a vector of strings (ipList)
    void loadIPs(const std::string& filename);

    // Sends ICMP echo requests to all loaded IPs
    void bendPackets(const std::string& ipStr);

    // Dumps raw memory in hex byte and printable split format
    void dump(const unsigned char *data_buffer, const unsigned int length);

    // Calculates the checksum of the ICMP header
    unsigned short icmp_checksum(unsigned short *addr, int len);

    // Calculates the CRC32 checksum
    int icmp_echo_header(int pack_no, int ttl);

    // Sends ICMP echo requests to all loaded IPs
    void send_icmp_echo_packet(void);

    // Receives ICMP echo replies from all loaded IPs
    void recv_icmp_reply_packet(void);

// private:
//    std::vector<std::string> ipList;

};

#endif // TRACE_HPP
