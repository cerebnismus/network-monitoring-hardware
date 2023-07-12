/**
 * @file icmp.cpp
 * @details helper functions for the bop-mon project.
 * @copyright oguzhan.ince@protonmail.com
 * @todo support osx and windows (currently only linux) use #ifdef __linux__ etc.
 */

/*
 * This program has to be run in root. 
 * This program is used to send ICMP echo request and receive ICMP echo reply.
 * https://tools.ietf.org/html/rfc792           "Internet Control Message Protocol"
 * https://tools.ietf.org/html/rfc791           "Internet Header Format"
 * https://tools.ietf.org/html/rfc760           "Internet Protocol"
 * https://www.iana.org/assignments/icmp-parameters/icmp-parameters.xhtml
 */

/*
 * You can capture packets by tcpdump like this:
 *         tcpdump -X -s0 -i eth0 -p icmp        
 */

#include <iostream>
#include <unistd.h> 
#include <fstream>
#include <cstring>  // memset etc.
#include <string>
#include <vector>

/*
#include <netinet/if_ether.h> // ETH_P_IP etc.
#include <netinet/ip.h>       // struct iphdr
#include <netinet/ip_icmp.h>  // struct icmphdr

#include <linux/if_packet.h>  // sockaddr_ll
#include <arpa/inet.h>        // inet_addr etc.
#include <net/if.h>           // if_nametoindex
*/

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
#include <netdb.h>
#include <setjmp.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>

#include "icmp.hpp"


#define PACKET_SIZE     4096
#define ICMP_DATA_LEN   56

PacketBender::PacketBender() {}
PacketBender::~PacketBender() {}

char sendpacket[PACKET_SIZE];           // store the send package
char recvpacket[PACKET_SIZE];           // store the recevice package

int sockfd;                             // store the socket file descriptor
int nsend = 0, nreceived = 0;           // store the number of send and receive packages

struct sockaddr_in dest_addr;           // store the destination address info
struct sockaddr_in from;                // store the localhost address info

struct timeval tm_recv;                 // store the time info when a package received
pid_t pid = getpid();                   // store the process id of main program


// loads IPs from a file into a vector of strings (ipList)
void PacketBender::loadIPs(const std::string& filename) {
    std::ifstream file(filename);
    std::string line;
    while (std::getline(file, line)) {
        ipList.push_back(line);
    }
    file.close();
}

// dumps raw memory in hex byte and printable split format
void PacketBender::dump(const unsigned char *data_buffer, const unsigned int length) {
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
  	}
  }
}



 /**********************************************************************
 * Function: cal_chksum                                                *
 * Description: calculate checksum                                     *
 * Arguments: 1st argument: pointer to beginning of the ICMP HEADER    *
 *            2nd argument: Length of ICMP packet header (64 bytes)    *
 **********************************************************************/
unsigned short PacketBender::icmp_checksum(unsigned short *addr, int len)
{
    int nleft = len;
    int sum = 0;
    unsigned short *w = addr;
    unsigned short answer = 0;
     /****************************************************************** 
     * The checksum is the 16-bit ones's complement of the one's       *
     * complement sum of the ICMP message starting with the ICMP Type. *
     * For computing the checksum , the checksum field should be zero. *
     ******************************************************************/
    while(nleft > 1) {
        sum += *w++;
        nleft -= 2;
    }
    if( nleft == 1) {
        *(unsigned char *)(&answer) = *(unsigned char *)w;
        sum += answer;
    }
    sum = (sum >> 16) + (sum & 0xffff);
    sum += (sum >> 16);
    answer = ~sum;
    return answer;
}

 /**********************************************************************
 * Function: ip_header                                                 *
 * Description: Craft IP header                                        *
 *                                                                     *
 *   0                   1                   2                   3     *
 *   0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1   *
 *  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+  *
 *  |Version|  IHL  |Type of Service|          Total Length         |  *
 *  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+  *
 *  |         Identification        |Flags|      Fragment Offset    |  *
 *  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+  *
 *  |  Time to Live |    Protocol   |         Header Checksum       |  *
 *  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+  *
 *  |                       Source Address                          |  *
 *  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+  *
 *  |                    Destination Address                        |  *
 *  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+  *
 *  |                    Options                    |    Padding    |  *
 *  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+  *
 *                                                                     *
 * *********************************************************************/


 /**********************************************************************
 * Function: icmp_echo_header                                          *
 * Description: Send "ICMP echo" packet to Destination Host            *
 *                                                                     *
 *   0                   1                   2                   3     *
 *   0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1   *
 *  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+  *
 *  |     Type      |     Code      |          Checksum             |  *
 *  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+  *
 *  |                             unused                            |  *
 *  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+  *
 *  |      Internet Header + 64 bits of Original Data Datagram      |  *
 *  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+  *
 *                                                                     *
 ***********************************************************************/
int PacketBender::icmp_echo_header(int pack_no)
{
    int packsize;
    struct icmp *icmp;
    struct timeval *tval;

    /* ICMP Header structure */
    icmp = (struct icmp*)sendpacket;
    icmp->icmp_type = ICMP_ECHO;
    icmp->icmp_code = 0;
    icmp->icmp_cksum = 0;
    icmp->icmp_seq = pack_no;
    icmp->icmp_id = pid;
    packsize = 8 + ICMP_DATA_LEN;  // 8 + 56 (data) = 64 Bytes ICMP header
    tval = (struct timeval *)icmp -> icmp_data;
    gettimeofday(tval, NULL);
    icmp->icmp_cksum = icmp_checksum( (unsigned short *)icmp, packsize);
    return packsize;
}

/*******************************************************************
 * Function: send_icmp_echo_packet                                 *
 * Description: Send "ICMP echo" packet to Destination Host        *
 *******************************************************************/
void PacketBender::send_icmp_echo_packet()
{
    int packetsize;

    nsend++;
    packetsize = icmp_echo_header(nsend);

    /* Send the ICMP packet to the destination Address */
    if( sendto(sockfd, sendpacket, packetsize, 0,
                (struct sockaddr *)&dest_addr, sizeof(dest_addr) ) < 0  ) {
        perror("sendto error");
        nsend--;
    }
}

/*******************************************************************
 * Function: recv_icmp_reply_packet                                *
 * Description: Receive "ICMP reply" packet from Destination Host  *
 *              and extract icmp header from icmp reply packet     *
 * Arguments:                                                      *
 *      buf : Pointer to received ICMP reply packet packet array.  *
 *      len : Length of the ICMP reply packet received.            *
 ******************************************************************/
void PacketBender::recv_icmp_reply_packet()
{
  /***********************************************************************
   * Be sure to receive a response to a previously issued ICMP packet     *
   * icmpid is the same as the PID of the process which has initiated the *
   * ICMP ping (i.e. ICMP echo) packet in first place.                    *
   ***********************************************************************/
    pause();

}

void PacketBender::bendPackets(const std::string& ipStr) {
    struct protoent *protocol;
    int size = 50 * 1024;
    int on = 1;

    /* Protocol: ICMP */
    if( (protocol = getprotobyname("icmp") ) == NULL) {
        perror("getprotobyname");
        exit(1);
    }

     // sockfd is a global variable
    if( (sockfd = socket(AF_INET, SOCK_RAW, protocol->p_proto) ) <= 0) {
    // if( (sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_RAW) ) <= 0) {
        perror("socket error");
        exit(1);
    }

    /* SO_RCVBUF
     * Sets or gets the maximum socket receive buffer in bytes. The
     * kernel doubles this value (to allow space for bookkeeping
     * overhead) when it is set using setsockopt(2), and this doubled
     * value is returned by getsockopt(2).
     */

    /* Manipulating socket options.  */
    setsockopt(sockfd, SOL_SOCKET, SO_RCVBUF, &size, sizeof(size) );
    /*
     * SO_DONTROUTE
     * Don't send via a gateway, send only to directly connected
     * hosts.
     */

    /************************************************************************* 
     * IN ORDER TO PING TO NETWORKS BEYOND THE GATEWAY, COMMENT OUT THE BELOW
     * LINE. 
     *************************************************************************/
    setsockopt(sockfd, SOL_SOCKET, SO_DONTROUTE, &on, sizeof(on));

    bzero(&dest_addr, sizeof(dest_addr));
    dest_addr.sin_family = AF_INET;
    dest_addr.sin_addr.s_addr = inet_addr(ipStr.c_str());

    
    printf("PING %s(%s): %d bytes data in ICMP packets.\n" , ipStr.c_str(),
            inet_ntoa(dest_addr.sin_addr), ICMP_DATA_LEN);
    
    while (1) {
        send_icmp_echo_packet();
        recv_icmp_reply_packet();
    }
}


/*

  // inform the kernel do not fill up the packet structure, we will build our own
  if(setsockopt(sd, IPPROTO_IP, IP_HDRINCL, val, sizeof(one)) < 0) {
    perror("setsockopt() error");
    exit(2);
  }
  printf("OK: socket option IP_HDRINCL is set.\n");

*/
