/**
 * @todo support osx and windows (currently only linux) use #ifdef __linux__ etc.
 * @todo dynamically get gateway mac address
 * @todo add more options to the icmp packet as possible as you can
 */

/*
 *  This program has to be run in root.
 *  This program is used to send ICMP echo request and receive ICMP echo reply.
 *  https://tools.ietf.org/html/rfc792           "Internet Control Message Protocol"
 *  https://tools.ietf.org/html/rfc791           "Internet Header Format"
 *  https://tools.ietf.org/html/rfc760           "Internet Protocol"
 *  https://www.iana.org/assignments/icmp-parameters/icmp-parameters.xhtml
 */

/*
 *  You can capture packets by wireshark, tshark or tcpdump like this:
 *         tcpdump -X -s0 -i eth0 -p icmp
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>           // memcpy, memset, etc.
#include <sys/socket.h>       // just one of them is enough
#include <netinet/if_ether.h> // Ethernet header
#include <netinet/ip.h>       // IP header
#include <netinet/ip_icmp.h>  // ICMP header
#include <linux/if_packet.h>  // AF_PACKET
#include <netinet/in.h>       // IPPROTO_ICMP
#include <unistd.h>           // close(), etc.
#include <arpa/inet.h>        // inet_addr(), etc.
#include <sys/ioctl.h>        // system calls
#include <net/if.h>           // struct ifreq (for getting MAC address)
#include <sys/select.h>       // select() (for timeout on recvfrom() function)
#include <time.h>             // time function for timestamp

// gettimeofday function for custom timestamp i think its worse than time function
#include <sys/time.h> 


// Compile with this command: (its for python flask metrics service)
// gcc -shared -o icmp.so -fPIC icmp.c

#define ip_destination "10.28.28.13" // Add your target IP

char *ip_gateway = NULL;
char *ip_source = NULL;

unsigned char *mac_destination_ptr = NULL;
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
        ip_source = inet_ntoa(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr);
        printf("IP: %s\n", ip_source);
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
    // printf("file descriptor closed\n");
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

    // Convert MAC address from string to array of unsigned char
    int values[6];
    int i;

    // Allocating memory for the MAC address bytes
    mac_destination_ptr = (unsigned char *)malloc(6 * sizeof(unsigned char));
    if (mac_destination_ptr == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(EXIT_FAILURE);
    }

    // Parsing the MAC address string into 6 integer values
    sscanf(mac_destination, "%x:%x:%x:%x:%x:%x", &values[0], &values[1], &values[2], &values[3], &values[4], &values[5]);

    // Converting the integer values into unsigned char
    for(i = 0; i < 6; ++i) {
        mac_destination_ptr[i] = (unsigned char) values[i];
    }
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
    int sockfd = socket(AF_PACKET, SOCK_RAW, ETH_P_IP);
    if (sockfd < 0) {
        perror("socket");
        return EXIT_FAILURE;
    }

    // ETHERNET II HEADER
    get_source();
    get_first_hop();
    char eth_packet[14]; // Ethernet II header is always 14 bytes
    struct ethhdr *eth = (struct ethhdr *) eth_packet;

    // Set destination and source MAC address
    eth->h_dest[0] = mac_destination_ptr[0];
    eth->h_dest[1] = mac_destination_ptr[1];
    eth->h_dest[2] = mac_destination_ptr[2];
    eth->h_dest[3] = mac_destination_ptr[3];
    eth->h_dest[4] = mac_destination_ptr[4];
    eth->h_dest[5] = mac_destination_ptr[5];
    eth->h_source[0] = mac_source_ptr[0];
    eth->h_source[1] = mac_source_ptr[1];
    eth->h_source[2] = mac_source_ptr[2];
    eth->h_source[3] = mac_source_ptr[3];
    eth->h_source[4] = mac_source_ptr[4];
    eth->h_source[5] = mac_source_ptr[5];

    printf("eth->h_dest[0] %02x \n", eth->h_dest[0]);
    printf("eth->h_dest[1] %02x \n", eth->h_dest[1]);
    printf("eth->h_dest[2] %02x \n", eth->h_dest[2]);
    printf("eth->h_dest[3] %02x \n", eth->h_dest[3]);
    printf("eth->h_dest[4] %02x \n", eth->h_dest[4]);
    printf("eth->h_dest[5] %02x \n", eth->h_dest[5]);

    printf("eth->h_source[0] %02x \n", eth->h_source[0]);
    printf("eth->h_source[1] %02x \n", eth->h_source[1]);
    printf("eth->h_source[2] %02x \n", eth->h_source[2]);
    printf("eth->h_source[3] %02x \n", eth->h_source[3]);
    printf("eth->h_source[4] %02x \n", eth->h_source[4]);
    printf("eth->h_source[5] %02x \n", eth->h_source[5]);

    // Indicate next header is IP
    eth->h_proto = htons(ETH_P_IP);

    // IP HEADER
    char ip_packet[20];
    struct iphdr *ip = (struct iphdr *) ip_packet;

    // Set IP header fields
    ip->ihl = 5; // Header length
    ip->version = 4; // IPv4
    ip->tos = 0; // Type of service
    ip->tot_len = htons(20 + 8); // Total length
    ip->id = htons(0x29A); // Identification
    ip->frag_off = 0x0000; // i cant set this to 0x4000 or 0x02 (dont know why)
    ip->ttl = MAXTTL; // Time to live
    ip->protocol = IPPROTO_ICMP; // Next protocol is ICMP
    ip->check = 0; // We will calculate the checksum later
    ip->saddr = inet_addr(ip_source); // Source IP address
    ip->daddr = inet_addr(ip_destination); // Destination IP address
    // TODO  : options and padding are not used for now
    // ISSUE : its look like im adding padding data to the packet 

    // Calculate IP checksum
    ip->check = checksum(ip_packet, 20);

    // ICMP HEADER
    char icmp_packet[8];
    struct icmphdr *icmp = (struct icmphdr *) icmp_packet;

    // Set ICMP header fields
    icmp->type = ICMP_ECHO; // ICMP type
    icmp->code = 0; // ICMP sub type
    icmp->un.echo.id = 0; // Identifier
    icmp->un.echo.sequence = 0; // Sequence number
    icmp->checksum = 0; // We will calculate the checksum later
    // ICMP data is not used for now

    // Calculate ICMP checksum
    icmp->checksum = checksum(icmp_packet, 8);

    // Adding a 64-bit timestamp to the ICMP data
    // uint64_t timestamp = (uint64_t)time(NULL); // 8 bytes

    // Construct FINAL PACKET
    char packet[42];
    memcpy(packet, eth_packet, 14);
    memcpy(packet + 14, ip_packet, 20);
    memcpy(packet + 34, icmp_packet, 8);
    // memcpy(packet + 42, &timestamp, 8);
    // instead of adding timestamp to the packet we can use gettimeofday function
    // gettimeofday function is may not be more accurate but its more faster than adding timestamp to the packet

    // Construct sockaddr_ll struct for sending packet out of interface index
    struct sockaddr_ll addr;
    int ifindex = if_nametoindex("eth0"); // TODO: Dynamic interface name from config
    addr.sll_ifindex = ifindex;           // Interface index number (eth0) (see "ifconfig" command)
    addr.sll_protocol = htons(ETH_P_ALL);  // Ethernet type (see /usr/include/linux/if_ether.h)
    addr.sll_family = AF_PACKET;          // Always AF_PACKET
    memcpy(addr.sll_addr, mac_destination, ETH_ALEN); // Destination MAC

    // Crafted packet in hex format
    printf("\nCrafted packet\n");
    print_raw_data(packet, sizeof(packet));

    // SELECT STRUCT
    fd_set readfds;
    FD_ZERO(&readfds);
    FD_SET(sockfd, &readfds);

    struct timeval timeout;
    timeout.tv_sec = 0;
    timeout.tv_usec = 10;

    // Receive Packet
    char recvpacket[42];
    struct sockaddr_in from;
    socklen_t recvsocklen = sizeof(from);


    // Using gettimeofday start time for latency calculation
    // send and receive time / 2 = latency (one way)
    struct timeval tv;
    gettimeofday(&tv, NULL);
    long long send_time_ms = tv.tv_sec*1000LL + tv.tv_usec/1000; // calculate milliseconds

    // SEND PACKET
    int sock_check_send = sendto(sockfd, packet, sizeof(packet), 0, (struct sockaddr *)&addr, sizeof(addr));
    printf("sock_check %d \n\n", sock_check_send);
    if (sock_check_send < 0) {
        perror("sendto");
        return EXIT_FAILURE;
    }

    // RECEIVE PACKET
    // for loop to receive packet when socket is ready
    for (int i = 0; i < 10; i++) {
        int retval = select(sockfd + 1, &readfds, NULL, NULL, &timeout);
        if (retval == -1) {
            perror("select()");
        } else if (retval) {
            printf("Data is available now.\n");
            int sock_check_recv = recvfrom(sockfd, recvpacket, sizeof(recvpacket), 0, (struct sockaddr *)(struct sockaddr *)&from, &recvsocklen);
            printf("sock_check %d \n\n", sock_check_recv);
            break;
        } else {
            printf("No data within timeout: %ld microseconds.\n", timeout.tv_usec);
            break;
        }
    }

    struct ethhdr *ethrecv = (struct ethhdr*) recvpacket;
    struct iphdr *iprecv = (struct iphdr*) (recvpacket + sizeof(struct ethhdr));
    struct icmphdr *icmprecv = (struct icmphdr*) (recvpacket + sizeof(struct ethhdr) + sizeof(struct iphdr));

    printf("Received packet\n");
    print_raw_data(recvpacket, sizeof(recvpacket));

    printf("# HELP bop_icmp_type int value of icmp type\n");
    printf("# TYPE bop_icmp_type summary\n");
    printf("bop_icmp_type{type=\"%d\"} %d\n", icmprecv->type, icmprecv->type); // first %d is for label and can be changed to string like ECHO_REPLY

    printf("# HELP bop_icmp_code int value of icmp code\n");
    printf("# TYPE bop_icmp_code summary\n");
    printf("bop_icmp_code{code=\"%d\"} %d\n", icmprecv->code, icmprecv->code); // first %d is for label and can be changed to string like ICMP_EXC_TTL

    // This will print the latency in scientific notation with 2 digits after the decimal point
    printf("# HELP bop_icmp_latency_calculated_microseconds duration of transmitting\n");
    printf("# TYPE bop_icmp_latency_calculated_microseconds gauge\n");

    ///////////////////////////
    printf("bop_icmp_latency_calculated_microseconds %ld\n", (end.tv_usec - start.tv_usec));

    close(sockfd);
    free(mac_destination_ptr);
    return EXIT_SUCCESS;
}