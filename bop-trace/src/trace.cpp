/**
 * @file trace.cpp
 * @details helper functions for the bop-trace project.
 * @copyright oguzhan.ince@protonmail.com
 */

#include <iostream>
#include <unistd.h>
#include <fstream>
#include <cstring>
#include <string>
#include <vector>

#include <net/if.h>
#include <linux/if_packet.h>
#include <net/ethernet.h>

#include <stdio.h>
#include <signal.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <unistd.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <netinet/ether.h>
#include <netdb.h>
#include <setjmp.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>

#include "trace.hpp"

#define ICMP_DATA_LEN 56
#define ICMP_PACKET 4096
#define PACKET_SIZE ICMP_PACKET + IP_MAXPACKET + ETHER_HDR_LEN

PacketBender::PacketBender() {}
PacketBender::~PacketBender() {}

char icmppacket[ICMP_PACKET]; // store the icmp package
char ippacket[IP_MAXPACKET];  // store the ip package

char sendpacket[PACKET_SIZE]; // store the send package
char recvpacket[PACKET_SIZE]; // store the recevice package

int sockfd;                   // store the socket file descriptor
int nsend = 0, nreceived = 0; // store the number of send and receive packages

struct sockaddr_in dest_addr; // store the destination address info
struct sockaddr_in from_addr; // store the localhost address info

pid_t pid = getpid(); // store the process id of main program

// loads IPs from a file into a vector of strings (ipList)
void PacketBender::loadIPs(const std::string &filename)
{
    std::ifstream file(filename);
    std::string line;
    while (std::getline(file, line))
    {
        ipList.push_back(line);
    }
    file.close();
}

// dumps raw memory in hex byte and printable split format
void PacketBender::dump(const unsigned char *data_buffer, const unsigned int length)
{
    unsigned char byte;
    unsigned int i, j;

    for (i = 0; i < length; i++)
    {

        byte = data_buffer[i];
        fprintf(stdout, "%02x ", data_buffer[i]); // display byte in hex

        if (((i % 16) == 15) || (i == length - 1))
        {
            for (j = 0; j < 15 - (i % 16); j++)
                fprintf(stdout, "   ");
            fprintf(stdout, "| ");

            for (j = (i - (i % 16)); j <= i; j++)
            { // display printable bytes from line
                byte = data_buffer[j];

                if ((byte > 31) && (byte < 127)) // outside printable char range
                    fprintf(stdout, "%c", byte);
                else
                    fprintf(stdout, ".");
            }
            fprintf(stdout, "\n"); // end of the dump line (each line 16 bytes)
        }
    }
}


unsigned short PacketBender::icmp_checksum(unsigned short *addr, int len)
{
    int nleft = len;
    int sum = 0;
    unsigned short *w = addr;
    unsigned short answer = 0;

    while (nleft > 1)
    {
        sum += *w++;
        nleft -= 2;
    }
    if (nleft == 1)
    {
        *(unsigned char *)(&answer) = *(unsigned char *)w;
        sum += answer;
    }
    sum = (sum >> 16) + (sum & 0xffff);
    sum += (sum >> 16);
    answer = ~sum;
    return answer;
}


int PacketBender::icmp_echo_header(int pack_no)
{
    int packsize;
    struct icmp *icmp;

    /* ICMP Header structure */
    icmp = (struct icmp *)icmppacket;
    icmp->icmp_type = ICMP_ECHO;
    icmp->icmp_code = 0;
    icmp->icmp_cksum = 0;
    icmp->icmp_seq = pack_no;
    icmp->icmp_id = pid;

    /* ICMP DATA structure */
    packsize = 8 + ICMP_DATA_LEN; 
    icmp->icmp_cksum = icmp_checksum((unsigned short *)icmp, packsize);

    /* IP Header structure */
    struct ip *ip;
    ip = (struct ip *)ippacket;
    ip->ip_v = IPVERSION; // low nibble
    ip->ip_hl = 5;        // high nibble (5 x 4 = 20 Bytes header length)
    ip->ip_tos = 0;       // type of service (0 = default) low delay
    ip->ip_len = sizeof(struct ip) + sizeof(struct icmp) + ICMP_DATA_LEN;
    ip->ip_id = pid;
    ip->ip_off = 0x0;        // fragment offset
    ip->ip_ttl = IPDEFTTL;   // time to live (default 64)  - IPTTLDEC
    ip->ip_p = IPPROTO_ICMP; // ICMP protocol number (1)
    ip->ip_sum = 0;
    ip->ip_src.s_addr = inet_addr("10.28.28.14");
    ip->ip_dst.s_addr = dest_addr.sin_addr.s_addr;
    // ip options are not needed right now (maybe later)
    packsize += sizeof(struct ip);
    ip->ip_sum = icmp_checksum((unsigned short *)ip, sizeof(struct ip));

    // Copy ip header and icmp header to buffer
    memcpy(sendpacket, ip, sizeof(struct ip));
    memcpy(sendpacket + sizeof(struct ip), icmp, sizeof(struct icmp));

    return packsize;
}

void PacketBender::send_icmp_echo_packet()
{
    int packetsize;
    nsend++;
    packetsize = icmp_echo_header(nsend);

    /* Send the ICMP packet to the destination Address */
    if (sendto(sockfd, sendpacket, packetsize, 0,
               (struct sockaddr *)&dest_addr, sizeof(dest_addr)) < 0)
    {
        perror("sendto error");
        nsend--;
    }
    else
    {
        printf("Send ICMP echo packet to %s\n", inet_ntoa(dest_addr.sin_addr));
    }
    close(sockfd);
}

void PacketBender::recv_icmp_reply_packet()
{
    struct sockaddr_ll saddrll;
    socklen_t recvsocklen = sizeof(saddrll);
    char recvpacket[4096];
    int rc;

    int receive_s = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
    if (receive_s < 0)
    {
        std::cout << "receive_s error!" << std::endl
                  << std::flush;
        exit(-1);
    }

    struct timeval tv;
    tv.tv_sec = 1;
    tv.tv_usec = 0;
    if (setsockopt(receive_s, SOL_SOCKET, SO_RCVTIMEO, (const char *)&tv, sizeof(tv)) < 0)
    {
        std::cout << "setsockopt RCVTIMEO error!" << std::endl
                  << std::flush;
        close(receive_s);
        exit(-1);
    }

    rc = recvfrom(receive_s, recvpacket, sizeof(recvpacket), 0, (struct sockaddr *)&saddrll, &recvsocklen);
    if (rc == -1)
    {
        std::cout << "recvfrom error!" << std::endl
                  << std::flush;
        close(receive_s);
        exit(-1);
    }
    close(receive_s);
    recvpacket[rc] = 0;

    std::cout << "receivedbytes= " << rc << std::flush;
    std::cout << saddrll.sll_addr << std::flush;
    std::cout << saddrll.sll_protocol << std::flush;
}

void PacketBender::bendPackets(const std::string &ipStr)
{
    struct protoent *protocol;
    int size = 50 * 1024;
    int on = 1;

    /* Protocol: ICMP */
    if ((protocol = getprotobyname("icmp")) == NULL)
    {
        perror("getprotobyname");
        exit(1);
    }

    // sockfd is a global variable
    if ((sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_RAW)) == -1)
    {
        perror("socket error");
        exit(1);
    }

    /* Manipulating socket options.  */
    setsockopt(sockfd, SOL_SOCKET, SO_RCVBUF, &size, sizeof(size));
    // setsockopt(sockfd, SOL_SOCKET, SO_DONTROUTE, &on, sizeof(on));

    bzero(&dest_addr, sizeof(dest_addr));
    dest_addr.sin_family = AF_INET;
    dest_addr.sin_addr.s_addr = inet_addr(ipStr.c_str());

    if (setsockopt(sockfd, IPPROTO_IP, IP_HDRINCL, &on, sizeof(on)) < 0)
    {
        perror("setsockopt() hdrinclude error");
        exit(2);
    }
    printf("OK: socket option IPPROTO_IP, IP_HDRINCL is set.\n");

    printf("PING %s(%s): %d bytes data in ICMP packets.\n", ipStr.c_str(),
           inet_ntoa(dest_addr.sin_addr), ICMP_DATA_LEN);

    send_icmp_echo_packet();
    recv_icmp_reply_packet();
}