// Compile with this command: (its for python flask metrics service)
// gcc -shared -o main.so -fPIC main.c

// icmp->un.echo.sequence += 1; // Sequence number (increment by 1 for each packet)

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
#include <pthread.h>          // for threads and mutex locks

int packet_count = 0;
int max_threads = 0;

pthread_mutex_t transmitMutex = PTHREAD_MUTEX_INITIALIZER;
unsigned int irreleventPings = 0;
unsigned int successfulPings = 0;
unsigned int failedPings = 0;

void logMessage(char *message) {
    time_t currentTime;
    struct tm *localTime;

    time(&currentTime);
    localTime = localtime(&currentTime);

    printf("%02d:%02d:%02d %s", localTime->tm_hour, localTime->tm_min, localTime->tm_sec, message);
}

#define ip_destination target_ip
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

    if (ioctl(fd, SIOCGIFADDR, &ifr) != -1) {  // Fetch the IP address
        ip_source = inet_ntoa(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr);
    }
 
    // Fetch the MAC address
    if (ioctl(fd, SIOCGIFHWADDR, &ifr) != -1) {
        mac_source_ptr = (unsigned char *)ifr.ifr_hwaddr.sa_data;
        sprintf(mac_source, "%02x:%02x:%02x:%02x:%02x:%02x",
            mac_source_ptr[0], mac_source_ptr[1], mac_source_ptr[2],
            mac_source_ptr[3], mac_source_ptr[4], mac_source_ptr[5]);
    }
    close(fd);
}


void get_first_hop() {
    char line[100];  // Execute "route" command and get the result in pipe
    FILE *fp = popen("route -n | grep 'UG[ \t]' | awk '{print $2}'", "r");
    if (fp == NULL) {
        perror("popen");
        pclose(fp);
        exit(EXIT_FAILURE);
    }

    while (fgets(line, sizeof(line), fp) != NULL) {  // Read the gateway IP from the result
        line[strcspn(line, "\n")] = 0; // Remove newline character
        ip_gateway = strdup(line);
    }
    pclose(fp);

    if (ip_gateway == NULL) {
        printf("IP: not found\n");
        free(ip_gateway);
        exit(EXIT_FAILURE);
    }

    char command[100];  // Construct a command to get the MAC address from the ARP table
    snprintf(command, sizeof(command), "grep '%s[ \t]' /proc/net/arp | awk '{print $4}'", ip_gateway);

    FILE *arp_fp = popen(command, "r");
    if (fgets(mac_destination, sizeof(mac_destination), arp_fp) != NULL) {
        mac_destination[strcspn(mac_destination, "\n")] = 0;  // Remove any trailing newline
    } else {
        printf("MAC: not found\n");
        pclose(arp_fp);
        exit(EXIT_FAILURE);
    }
    pclose(arp_fp);

    int i, values[6];  // Convert MAC address from string to array of unsigned char
    mac_destination_ptr = (unsigned char *)malloc(6 * sizeof(unsigned char));
    if (mac_destination_ptr == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(EXIT_FAILURE);
    }

    sscanf(mac_destination, "%x:%x:%x:%x:%x:%x", &values[0], &values[1], &values[2], &values[3], &values[4], &values[5]);
    for(i = 0; i < 6; ++i) {  // Converting the integer values into unsigned char
        mac_destination_ptr[i] = (unsigned char) values[i];
    }
}

// ICMP Checksum computation
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


void *pingFunction(void *target_ip) {

    int sockfd_send = socket(PF_PACKET, SOCK_RAW, ETH_P_IP);
    if (sockfd_send == -1) {
        logMessage("Failed to create socket");
        pthread_exit(0);
    }

    get_source();
    get_first_hop();
    char eth_packet[14]; // Ethernet II header
    struct ethhdr *eth = (struct ethhdr *) eth_packet;

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
    eth->h_proto = htons(ETH_P_IP);

    char ip_packet[20]; // IP header
    struct iphdr *ip = (struct iphdr *) ip_packet;

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
    ip->check = checksum(ip_packet, 20); // Calculate IP checksum

    char icmp_packet[8];
    struct icmphdr *icmp = (struct icmphdr *) icmp_packet;

    icmp->type = ICMP_ECHO; // ICMP type
    icmp->code = 0; // ICMP sub type
    icmp->checksum = 0; // We will calculate the checksum later
    icmp->un.echo.id = 0; // Identifier
    icmp->un.echo.sequence = htons(1); // Sequence number
    icmp->checksum = checksum(icmp_packet, 8); // Calculate ICMP checksum

    char sendpacket[42];  // Construct Packet
    memcpy(sendpacket, eth_packet, 14);
    memcpy(sendpacket + 14, ip_packet, 20);
    memcpy(sendpacket + 34, icmp_packet, 8);

    struct sockaddr_ll addr;
    int ifindex = if_nametoindex(iface);  // TODO: Dynamic interface name from argv
    addr.sll_ifindex = ifindex;           // Interface index number (see "ifconfig" command)
    addr.sll_protocol = htons(ETH_P_IP);  // Ethernet type (see /usr/include/linux/if_ether.h)
    addr.sll_family = PF_PACKET;
    memcpy(addr.sll_addr, mac_destination, ETH_ALEN);

    char recvpacket[28]; // RECEIVER SOCKET SETUP
    struct sockaddr_in from;
    from = (struct sockaddr_in){.sin_family = AF_INET, .sin_port = htons(0), .sin_addr = inet_addr(ip_destination)};
    socklen_t recvsocklen = sizeof(from);

    int sockfd_recv = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
    if (sockfd_recv == -1) {
        logMessage("Failed to create socket");
        pthread_exit(0);
    }


    for (int i = 0; i < packet_count; i++) {
        if (sendto(sockfd_send, sendpacket, sizeof(sendpacket), 0, (struct sockaddr *)&addr, sizeof(addr)) != -1) {

            // logMessage("Ping sending \n");
            pthread_mutex_lock(&transmitMutex);

            struct timeval start, end;  // its for rtt
            gettimeofday(&start, NULL); // calculate milliseconds for response time

            struct timeval timeout;
            timeout.tv_sec = 1;
            timeout.tv_usec = 0;
            setsockopt(sockfd_recv, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));

            while (1) {

                if (recvfrom(sockfd_recv, recvpacket, sizeof(recvpacket), 0, (struct sockaddr *)(struct sockaddr *)&from, &recvsocklen) != -1) {

                    struct iphdr *iprecv = (struct iphdr *)recvpacket;
                    struct icmphdr *icmprecv = (struct icmphdr *)(recvpacket + (iprecv->ihl << 2));

                    if (htons(icmprecv->un.echo.id) == 0) {
                        gettimeofday(&end, NULL);  // Store end time
                        long seconds = end.tv_sec - start.tv_sec;
                        long microseconds = end.tv_usec - start.tv_usec;

                        if (microseconds < 0) {
                            seconds--;
                            microseconds += 1000000;
                        }
                        double elapsed = seconds + microseconds * 1e-6 * 1000;
                        int elapsed_int = (int)(elapsed * 1000);
                        // printf("Response time: %.3f ms\n\n", elapsed);
                        // printf("Response time: %.4e \n\n", elapsed_int);
                        // 64 bytes from 8.8.8.8: icmp_seq=4 ttl=60 time=33.2 ms
                        printf("60 bytes from %s: icmp_seq=%d ttl=%d time=%.3f ms\n", ip_destination, htons(icmp->un.echo.sequence), ip->ttl, elapsed);
                        successfulPings++;
                        break;
                    } else {
                        logMessage("Ping recv irrelevent \n");
                        irreleventPings++;
                    }

                } else {
                    logMessage("Ping recv failed \n");
                    failedPings++;
                    break;
                }

            } // end of while loop

            pthread_mutex_unlock(&transmitMutex);

        } else {
            logMessage("Ping send failed \n");
            pthread_mutex_lock(&transmitMutex);
            failedPings++;
            pthread_mutex_unlock(&transmitMutex);
        }

    }
    close(sockfd_send);
    close(sockfd_recv);
    pthread_exit(0);
}


int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <target_ip> <packet_count>\n", argv[0]);
        return 1;
    }

    char *target_ip = argv[1];
    max_threads = atoi(argv[2]);
    packet_count = atoi(argv[2]);

    if (packet_count <= 0) {
        fprintf(stderr, "Invalid number of packets. Must be greater than 0.\n");
        return 1;
    }

    if (max_threads <= 0) {
        fprintf(stderr, "Invalid number of threads. Must be greater than 0.\n");
        return 1;
    }
    pthread_t threads[max_threads];

    // Spawn threads to perform pinging.
    for (int i = 0; i < max_threads; i++) {
        if (pthread_create(&threads[i], NULL, pingFunction, target_ip)) {
            fprintf(stderr, "Error creating thread\n");
            return 1;
        }
    }

    // This code will actually never reach here.
    for (int i = 0; i < max_threads; i++) {
        printf("Joining thread %d\n", i);
        pthread_join(threads[i], NULL);
    }


    printf("Metrics: Successful pings = %u, Failed pings = %u\n", successfulPings, failedPings);


    return 0;
}



    /*
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

    //printf("# HELP bop_icmp_ip_saddr IP source address\n");
    //printf("# TYPE bop_icmp_ip_saddr gauge\n");
    //printf("bop_icmp_ip_saddr %s\n", inet_ntoa(*(struct in_addr *)&iprecv->saddr));

    //printf("# HELP bop_icmp_ip_daddr IP destination address\n");
    //printf("# TYPE bop_icmp_ip_daddr gauge\n");
    //printf("bop_icmp_ip_daddr %s\n", inet_ntoa(*(struct in_addr *)&iprecv->daddr));

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
    */