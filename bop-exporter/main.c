#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/if_ether.h> // Ethernet header
#include <netinet/ip.h>       // IP header
#include <netinet/ip_icmp.h>  // ICMP header
#include <linux/if_packet.h>  // AF_PACKET
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/time.h>

#include <sys/ioctl.h>
#include <net/if.h>

// Compile with this command: (its for python flask metrics service)
// gcc -shared -o icmp_ping.so -fPIC icmp_ping.c

#define ip_destination "10.28.28.13" // Add your target IP

char *ip_gateway = NULL;
struct sockaddr_in *ip_source = NULL;
char mac_destination[50];
unsigned char *mac_source_ptr = NULL;
char mac_source[50];

void
get_source() {
    struct ifreq ifr;
    int fd = socket(AF_INET, SOCK_DGRAM, 0);

    // Provide the name of the network interface you're interested in
    strcpy(ifr.ifr_name, "eth0");

    printf("Source Addresses\n");
    printf("----------------\n");

    // Fetch the IP address
    if (ioctl(fd, SIOCGIFADDR, &ifr) != -1) {
        ip_source = (struct sockaddr_in *)&ifr.ifr_addr;
        printf("IP: %s\n", inet_ntoa(ip_source->sin_addr));
    } else {
        perror("ioctl");
    }

    // Fetch the MAC address
    if (ioctl(fd, SIOCGIFHWADDR, &ifr) != -1) {
        mac_source_ptr = (unsigned char *)ifr.ifr_hwaddr.sa_data;
        sprintf(mac_source, "%02x:%02x:%02x:%02x:%02x:%02x",
            mac_source_ptr[0], mac_source_ptr[1], mac_source_ptr[2],
            mac_source_ptr[3], mac_source_ptr[4], mac_source_ptr[5]);
        printf("MAC: %s\n\n", mac_source);
    } else {
        perror("ioctl");
    }
    close(fd);
}


void
get_first_hop() {
    char line[100];
    // Execute "route" command and get the result in pipe
    FILE *fp = popen("route -n | grep 'UG[ \t]' | awk '{print $2}'", "r");
    if (fp == NULL) {
        perror("popen");
        pclose(fp);
        exit(EXIT_FAILURE);
    }

    // Read the gateway IP from the result
    while (fgets(line, sizeof(line), fp) != NULL) {
        line[strcspn(line, "\n")] = 0; // Remove newline character
        ip_gateway = strdup(line);
    }
    pclose(fp);

    if (ip_gateway != NULL) {
        printf("Default gateway\n");
        printf("---------------\n");
        printf("IP: %s\n", ip_gateway);
        // free(ip_gateway);
    } else {
        printf("IP: not found\n");
        free(ip_gateway);
        exit(EXIT_FAILURE);
    }

    // Construct a command to get the MAC address from the ARP table
    char command[100];
    snprintf(command, sizeof(command), "grep '%s[ \t]' /proc/net/arp | awk '{print $4}'", ip_gateway);
    // printf("Command: %s\n", command);

    FILE *arp_fp = popen(command, "r");
    if (fgets(mac_destination, sizeof(mac_destination), arp_fp) != NULL) {
        // Remove any trailing newline
        mac_destination[strcspn(mac_destination, "\n")] = 0;
        printf("MAC: %s\n\n", mac_destination);
    } else {
        printf("MAC: not found\n");
        pclose(arp_fp);
        exit(EXIT_FAILURE);
    }
    pclose(arp_fp);
}

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

    // TODO: this needs to be in for loop to run continously outside the main function

    // Socket file descriptor for sending/receiving packets
    // sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
    // sockfd = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
    int sockfd = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_IP));
    if (sockfd < 0) {
        perror("socket");
        return EXIT_FAILURE;
    }


    // Ethernet packet
    get_source();
    get_first_hop();
    char eth_packet[14];
    struct ethhdr *eth = (struct ethhdr *) eth_packet;

    // Set destination and source MAC address
    memcpy(eth->h_dest, mac_destination, ETH_ALEN);
    memcpy(eth->h_source, mac_source, ETH_ALEN);

    eth->h_proto = htons(ETH_P_IP); // Indicate next header is IP

    // IP packet
    char ip_packet[20];
    struct iphdr *ip = (struct iphdr *) ip_packet;

    // Set IP header fields
    ip->ihl = 5; // Header length
    ip->version = 4; // IPv4
    ip->tos = 0; // Type of service
    ip->tot_len = htons(20 + 8); // Total length
    ip->id = htons(0x1234); // Identification
    ip->frag_off = 0; // Fragment offset
    ip->ttl = 64; // Time to live
    ip->protocol = IPPROTO_ICMP; // Next protocol is ICMP
    ip->check = 0; // We will calculate the checksum later
    ip->saddr = inet_addr(inet_ntoa(ip_source->sin_addr)); // Source IP address
    ip->daddr = inet_addr(ip_destination); // Destination IP address
    // options and padding are not used here
    
    // Calculate IP checksum
    ip->check = checksum(ip_packet, 20);

    // ICMP packet
    char icmp_packet[8];
    struct icmphdr *icmp = (struct icmphdr *) icmp_packet;

    // Set ICMP header fields
    icmp->type = ICMP_ECHO; // ICMP type
    icmp->code = 0; // ICMP sub type
    icmp->un.echo.id = 0; // Identifier
    icmp->un.echo.sequence = 0; // Sequence number
    icmp->checksum = 0; // We will calculate the checksum later
    
    // Calculate ICMP checksum
    icmp->checksum = checksum(icmp_packet, 8);

    // Construct final packet
    char packet[42];
    memcpy(packet, eth_packet, 14);
    memcpy(packet + 14, ip_packet, 20);
    memcpy(packet + 34, icmp_packet, 8);

    // craft packet
    int ifindex = if_nametoindex("eth0"); // Replace with the correct interface name

    struct sockaddr_ll addr;
    memset(&addr, 0, sizeof(addr));
    addr.sll_ifindex = ifindex;
    addr.sll_protocol = htons(ETH_P_IP); // Specify the next protocol (IP in this case)
    addr.sll_family = AF_PACKET;
    // addr.sll_halen = ETH_ALEN; // MAC address length is 6 bytes
    // memcpy(addr.sll_addr, mac_destination, ETH_ALEN); // Destination MAC
    // addr.sll_addr[6] = 0x00; // Not used
    // addr.sll_addr[7] = 0x00; // Not used

    // addr.sin_port = 0; // Not needed in SOCK_RAW
    // addr.sin_addr.s_addr = inet_addr(ip_destination);
    addr.sll_halen = ETH_ALEN; // MAC address length is 6 bytes
    memcpy(addr.sll_addr, mac_destination, ETH_ALEN); // Destination MAC
    memcpy(addr.sll_addr, mac_source, ETH_ALEN); // Source MAC

    // Send packet
    int sock_check = sendto(sockfd, packet, sizeof(packet), 0, (struct sockaddr *)&addr, sizeof(addr));
    printf("sock_check %d \n", sock_check);
    if (sock_check < 0) {
        perror("sendto");
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

/*
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

*/

    close(sockfd);
    return EXIT_SUCCESS;
}
