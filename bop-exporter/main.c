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
 *         tcpdump -X -s0 -i en3 -p icmp
 *         tcpdump -X -s0 -i eth0 -p icmp
 

SOCKET CREATION - 1024 per process for non-root users (ulimit -n)
Root users can create about 65,000 sockets per process. Which is the maximum number of ports.

The maximum number of sockets that can be created in a Linux system depends on various factors, 
such as the system's available resources and kernel parameters. However, a common limit set 
for the maximum number of file descriptors, which includes sockets, is 1024 per process 
for user-level applications. This can usually be viewed or modified using the ulimit command.

ulimit -n [new limit]

In Red Hat Enterprise Linux (RHEL), the maximum number of open files (which includes sockets) 
is often set to 1024 per process for non-root users, similar to other Linux distributions. 
You can check the current limit using the ulimit -n command.

For permanent changes, you can edit the /etc/security/limits.conf file to set hard and soft limits. 
The format in limits.conf would look something like:

username soft nofile 4096
username hard nofile 8192

You can also set these limits for groups or for all users.
Another way to modify this setting is to directly interact with systemd by using the systemctl command, 
although this usually applies to services rather than user sessions.
For more comprehensive control, especially in a large-scale enterprise environment, 
you may also use centralized management tools that RHEL supports, such as Red Hat Satellite.
Be sure to consult the specific documentation for your RHEL version as features and configuration options can change.

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
#include <sys/time.h> 

#define ip_destination "8.8.8.8" // Add your target IP
#define iface "eth0" // Add your interface name

char *ip_gateway = NULL;
char *ip_source = NULL;

unsigned char *mac_destination_ptr = NULL;
char mac_destination[50];
unsigned char *mac_source_ptr = NULL;
char mac_source[50];


void get_source() {
    struct ifreq ifr;
    int fd = socket(AF_INET, SOCK_DGRAM, 0);

    // Provide the name of the network interface you're interested in
    strcpy(ifr.ifr_name, iface);

    // printf("Source Addresses\n");
    // printf("----------------\n");

    // Fetch the IP address
    if (ioctl(fd, SIOCGIFADDR, &ifr) != -1) {
        ip_source = inet_ntoa(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr);
        // printf("IP: %s\n", ip_source);
    } else {
        perror("ioctl");
    }
 
    // Fetch the MAC address
    if (ioctl(fd, SIOCGIFHWADDR, &ifr) != -1) {
        mac_source_ptr = (unsigned char *)ifr.ifr_hwaddr.sa_data;
        sprintf(mac_source, "%02x:%02x:%02x:%02x:%02x:%02x",
            mac_source_ptr[0], mac_source_ptr[1], mac_source_ptr[2],
            mac_source_ptr[3], mac_source_ptr[4], mac_source_ptr[5]);
        // printf("MAC: %s\n\n", mac_source);
    } else {
        perror("ioctl");
    }
    close(fd);
    // printf("file descriptor closed\n");
}


void get_first_hop() {
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
        // printf("Default gateway\n");
        // printf("---------------\n");
        // printf("IP: %s\n", ip_gateway);
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
        // printf("MAC: %s\n\n", mac_destination);
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

unsigned short checksum(void *b, int len) {
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

void print_raw_data(unsigned char *data, int length) {
    int i;
    printf("  0x0000:  ");
    for(i = 0; i < length; i++) { // Print each 2 byte in hexadecimal with a space in the end (e.g. 0a1f 2c3b ...)
        printf("%02x", data[i]);
        if ((i + 1) % 2 == 0) {
            printf(" ");
        }
        if (i == 15) {
            printf("\n  0x0010:  ");
        }
        if (i == 31) {
            printf("\n  0x0020:  ");
        }
    }
    printf("\n");
}


int main() {
    // TODO: this needs to be in for loop to run continously outside the main function
    int sockfd = socket(AF_PACKET, SOCK_RAW, ETH_P_IP);
    if (sockfd < 0) {
        perror("socket");
        exit(EXIT_FAILURE);
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

    // Indicate next header is IP
    eth->h_proto = htons(ETH_P_IP);

/*
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
*/

    // IP HEADER
    char ip_packet[20];
    struct iphdr *ip = (struct iphdr *) ip_packet;

    // Set IP header fields
    ip->ihl = 5; // Header length
    ip->version = 4; // IPv4
    ip->tos = 0; // Type of service
    ip->tot_len = htons(20 + 8); // Total length
    ip->id = htons(1); // Identification
    ip->frag_off = 0x0000; // i cant set this to 0x4000 or 0x02 (dont know why)
    ip->ttl = IPDEFTTL; // Time to live
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

    // Construct FINAL PACKET
    char packet[42];
    memcpy(packet, eth_packet, 14);
    memcpy(packet + 14, ip_packet, 20);
    memcpy(packet + 34, icmp_packet, 8);
    // memcpy(packet + 42, &timestamp, 8);

    // Construct sockaddr_ll struct for sending packet out of interface index
    struct sockaddr_ll addr;
    int ifindex = if_nametoindex(iface);  // TODO: Dynamic interface name from config
    addr.sll_ifindex = ifindex;           // Interface index number (see "ifconfig" command)
    addr.sll_protocol = htons(ETH_P_IP);  // Ethernet type (see /usr/include/linux/if_ether.h)
    addr.sll_family = AF_PACKET;          // Always AF_PACKET
    memcpy(addr.sll_addr, mac_destination, ETH_ALEN); // Destination MAC

    struct timeval start, end;    
    gettimeofday(&start, NULL); // calculate milliseconds

    // SEND PACKET
    int sock_check_send = sendto(sockfd, packet, sizeof(packet), 0, (struct sockaddr *)&addr, sizeof(addr));
    if (sock_check_send < 0) {
        perror("sendto");
        exit(EXIT_FAILURE);
    }
    // printf("\nsock_check_send %d \n", sock_check_send);
    // print_raw_data(packet, sizeof(packet));

    // RECEIVE PACKET
    char recvpacket[28];
    struct sockaddr_in from;
    from = (struct sockaddr_in){.sin_family = AF_INET, .sin_port = htons(0), .sin_addr = inet_addr(ip_destination)};
    socklen_t recvsocklen = sizeof(from);

    int sockfd_recv = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
    if (sockfd_recv < 0) {
        perror("socket");
        return EXIT_FAILURE;
    }

    int sock_check_recv = recvfrom(sockfd_recv, recvpacket, sizeof(recvpacket), 0, (struct sockaddr *)(struct sockaddr *)&from, &recvsocklen);
    if (sock_check_recv < 0) {
        perror("recvfrom");
        return EXIT_FAILURE;
    }
    // printf("\nsock_check_recv %d \n", sock_check_recv);
    // print_raw_data(recvpacket, sizeof(recvpacket));

    gettimeofday(&end, NULL);  // Store end time
    long seconds = end.tv_sec - start.tv_sec;
    long microseconds = end.tv_usec - start.tv_usec;

    if (microseconds < 0) {
        seconds--;
        microseconds += 1000000;
    }

    double elapsed = seconds + microseconds * 1e-6 * 1000;
    // printf("Response time: %.3f ms\n\n", elapsed);
    int elapsed_int = (int)(elapsed * 1000); 
    // printf("Response time: %.4e \n\n", elapsed_int);
    // sample -> 9.63147392e+09 output -> 3.0691e+01

    // print recvpacket
    struct iphdr *iprecv = (struct iphdr *)recvpacket;
    struct icmphdr *icmprecv = (struct icmphdr *)(recvpacket + (iprecv->ihl << 2));

    // Display metrics for Prometheus
    printf("# HELP bop_icmp_ip_version IP header version\n");
    printf("# TYPE bop_icmp_ip_version gauge\n");
    printf("bop_icmp_ip_version %d\n", iprecv->version);

    printf("# HELP bop_icmp_ip_ihl IP header IHL\n");
    printf("# TYPE bop_icmp_ip_ihl gauge\n");
    printf("bop_icmp_ip_ihl %d\n", iprecv->ihl);

    printf("# HELP bop_icmp_ip_tos IP header TOS\n");
    printf("# TYPE bop_icmp_ip_tos gauge\n");
    printf("bop_icmp_ip_tos %d\n", iprecv->tos);

    printf("# HELP bop_icmp_ip_tot_len IP total length\n");
    printf("# TYPE bop_icmp_ip_tot_len gauge\n");
    printf("bop_icmp_ip_tot_len %d\n", ntohs(iprecv->tot_len));

    printf("# HELP bop_icmp_ip_frag_off IP fragment offset\n");
    printf("# TYPE bop_icmp_ip_frag_off gauge\n");
    printf("bop_icmp_ip_frag_off %d\n", iprecv->frag_off);

    printf("# HELP bop_icmp_ip_ttl IP time to live\n");
    printf("# TYPE bop_icmp_ip_ttl gauge\n");
    printf("bop_icmp_ip_ttl %d\n", iprecv->ttl);

    printf("# HELP bop_icmp_ip_protocol IP protocol\n");
    printf("# TYPE bop_icmp_ip_protocol gauge\n");
    printf("bop_icmp_ip_protocol %d\n", iprecv->protocol);

    printf("# HELP bop_icmp_ip_saddr IP source address\n");
    printf("# TYPE bop_icmp_ip_saddr gauge\n");
    printf("bop_icmp_ip_saddr %s\n", inet_ntoa(*(struct in_addr *)&iprecv->saddr));

    printf("# HELP bop_icmp_ip_daddr IP destination address\n");
    printf("# TYPE bop_icmp_ip_daddr gauge\n");
    printf("bop_icmp_ip_daddr %s\n", inet_ntoa(*(struct in_addr *)&iprecv->daddr));

    printf("# HELP bop_icmp_type int value of icmp type\n");
    printf("# TYPE bop_icmp_type summary\n");
    printf("bop_icmp_type{type=\"%d\"} %d\n", icmprecv->type, icmprecv->type);

    printf("# HELP bop_icmp_code int value of icmp code\n");
    printf("# TYPE bop_icmp_code summary\n");
    printf("bop_icmp_code{code=\"%d\"} %d\n", icmprecv->code, icmprecv->code);

    printf("# HELP bop_icmp_response_calculated_milliseconds duration of transmitting packet\n");
    printf("# TYPE bop_icmp_response_calculated_milliseconds gauge\n");
    printf("bop_icmp_response_calculated_milliseconds %.4e\n", elapsed_int);

    printf("# HELP bop_icmp_sin_family sin_family of icmp response\n");
    printf("# TYPE bop_icmp_sin_family gauge\n");
    printf("bop_icmp_sin_family %d\n", from.sin_family);

    printf("# HELP bop_icmp_sin_port sin_port of icmp response\n");
    printf("# TYPE bop_icmp_sin_port gauge\n");
    printf("bop_icmp_sin_port %d\n", from.sin_port);

    close(sockfd);
    close(sockfd_recv);
    free(mac_destination_ptr);
    return EXIT_SUCCESS;
}