#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/if_ether.h> // Ethernet header
#include <netinet/ip.h>       // IP header
#include <netinet/ip_icmp.h>  // ICMP header
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/time.h>

// Compile with this command: (its for python flask metrics service)
// gcc -shared -o icmp_ping.so -fPIC icmp_ping.c

#define TARGET "10.28.28.13" // Add your target IP
int bop_icmp_received_counter_total = 0;

unsigned short 
checksum(void *b, int len) {
    unsigned short *buf = b;
    unsigned int sum = 0;
    unsigned short result;

    for (sum = 0; len > 1; len -= 2)
        sum += *buf++;
    if (len == 1)
        sum += *(unsigned char *)buf;
    sum = (sum >> 16) + (sum & 0xFFFF);
    sum += (sum >> 16);
    result = ~sum;
    return result;
}

void 
print_raw_data(unsigned char *data, int length) {
    int i;
    for(i = 0; i < length; i++) {
        printf("%02x ", data[i]); // Print each byte in hexadecimal
        if ((i + 1) % 16 == 0) { // Format nicely, 16 bytes per line
            printf("\n");
        }
    }
    printf("\n");
}

int 
main() {

    // this needs to be in for loop to run continously outside the main function
    struct icmphdr icmp_hdr;
    struct sockaddr_in addr;
    int sockfd, ret, ret2;
    char packet[8];

    // latency metrics variables
    struct timeval start, end;
    double latency;

    // sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
    sockfd = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
    if (sockfd < 0) {
        perror("socket");
        return EXIT_FAILURE;
    }

    // Determine Next Hop (gateway) MAC Address


    struct ethhdr *eth = (struct ethhdr *) packet;

    // Set destination and source MAC address
    // You will need to fill these with correct values
    memcpy(eth->h_dest, DEST_MAC, ETH_ALEN);
    memcpy(eth->h_source, SRC_MAC, ETH_ALEN);

    eth->h_proto = htons(ETH_P_IP); // Indicate next header is IP



    addr.sin_family = AF_INET;
    inet_pton(AF_INET, TARGET, &addr.sin_addr);

    memset(&icmp_hdr, 0, sizeof(icmp_hdr));
    icmp_hdr.type = ICMP_ECHO;
    icmp_hdr.un.echo.id = htons(0x1234);
    icmp_hdr.checksum = checksum(&icmp_hdr, sizeof(icmp_hdr));

    memcpy(packet, &icmp_hdr, sizeof(icmp_hdr));

    gettimeofday(&start, NULL);
    ret = sendto(sockfd, packet, sizeof(packet), 0, (struct sockaddr *)&addr, sizeof(addr));
    if (ret <= 0) {
        perror("sendto");
        return EXIT_FAILURE;
    }
    // Receive and handle response for expose imcp metrics
    char buffer[64] = {0};
    struct sockaddr_in servaddr;
    int len = sizeof(servaddr);
    ret2 = recvfrom(sockfd, buffer, sizeof(buffer), 0, (struct sockaddr *)&servaddr, &len);
    if (ret2 <= 0) {
        perror("recvfrom");
        return EXIT_FAILURE;
    }

    /***********************************************************************
     * Format: icmp_echo_header                                            *
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


    /**********************************************************************
     * Format: ip_header                                                   *
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
     **********************************************************************/

    // get the response icmp + ip packet header
    // TODO: checksum validation for icmp and ip
    struct icmphdr *icmp_hdr_response = (struct icmphdr *)buffer;
    struct iphdr *ip_hdr_response = (struct iphdr *)buffer;

    print_raw_data(buffer, sizeof(buffer));


    // get the icmp type and code
    int type = icmp_hdr_response->type;
    int code = icmp_hdr_response->code;

    // get the ip source and destination address
    char source_ip[INET_ADDRSTRLEN];
    char dest_ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &(ip_hdr_response->saddr), source_ip, INET_ADDRSTRLEN);
    inet_ntop(AF_INET, &(ip_hdr_response->daddr), dest_ip, INET_ADDRSTRLEN);

    //get all the ip header fields
    int version = ip_hdr_response->version;
    int ihl = ip_hdr_response->ihl;
    int tos = ip_hdr_response->tos;
    int tot_len = ip_hdr_response->tot_len;
    int id = ip_hdr_response->id;
    int frag_off = ip_hdr_response->frag_off;
    int ttl = ip_hdr_response->ttl;

    // print for debug
    printf("ICMP type: %d\n", type);
    printf("ICMP code: %d\n", code);
    printf("IP source: %s\n", source_ip);
    printf("IP dest: %s\n", dest_ip);




    // icmp metrics
    printf("# HELP bop_icmp_type int value of icmp type\n");
    printf("# TYPE bop_icmp_type summary\n");
    printf("bop_icmp_type{type=\"%d\"} %d\n", type, type); // first %d is for label and can be changed to string like ECHO_REPLY

    printf("# HELP bop_icmp_code int value of icmp code\n");
    printf("# TYPE bop_icmp_code summary\n");
    printf("bop_icmp_code{code=\"%d\"} %d\n", code, code); // first %d is for label and can be changed to string like ICMP_EXC_TTL





    // calculate latency
    gettimeofday(&end, NULL);
    latency = (end.tv_usec - start.tv_usec);
    latency += (end.tv_usec - start.tv_usec);

    // This will print the latency in scientific notation with 2 digits after the decimal point
    printf("# HELP bop_icmp_latency_calculated_microseconds duration of transmitting\n");
    printf("# TYPE bop_icmp_latency_calculated_microseconds gauge\n");
    printf("bop_icmp_latency_calculated_microseconds %.2e\n", latency);



    close(sockfd);
    return EXIT_SUCCESS;
}
