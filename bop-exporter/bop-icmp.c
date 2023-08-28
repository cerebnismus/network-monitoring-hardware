#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip_icmp.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <time.h>

#define MAX_THREADS 5 // Maximum number of threads to spawn for pinging.
                      // It can be get from the user, argv[2] in this example.

pthread_mutex_t icmpMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t metricsMutex = PTHREAD_MUTEX_INITIALIZER;

unsigned int successfulPings = 0;
unsigned int failedPings = 0;
 
// Simplified logging function
void logMessage(char *message) {
    time_t currentTime;
    struct tm *localTime;

    time(&currentTime);
    localTime = localtime(&currentTime);

    printf("%02d:%02d:%02d %s\n", localTime->tm_hour, localTime->tm_min, localTime->tm_sec, message);
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
    int sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
    struct sockaddr_in servaddr;
    char packet[64];
    struct icmphdr *icmpHeader;

    if (sockfd == -1) {
        logMessage("Failed to create socket");
        pthread_exit(0);
    }

    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = 0;
    servaddr.sin_addr.s_addr = inet_addr(target_ip);

    icmpHeader = (struct icmphdr *)packet;
    icmpHeader->type = ICMP_ECHO;
    icmpHeader->code = 0;
    icmpHeader->checksum = 0;
    icmpHeader->un.echo.id = 0;
    icmpHeader->un.echo.sequence = 1;

    icmpHeader->checksum = checksum(packet, sizeof(packet));

    while (1) {
        if (sendto(sockfd, packet, sizeof(packet), 0, (struct sockaddr *)&servaddr, sizeof(servaddr)) == -1) {
            logMessage("Ping failed.");
            pthread_mutex_lock(&metricsMutex);
            failedPings++;
            pthread_mutex_unlock(&metricsMutex);
        } else {
            logMessage("Ping sent.");
            pthread_mutex_lock(&metricsMutex);
            successfulPings++;
            pthread_mutex_unlock(&metricsMutex);
        }

        sleep(1);
    }

    close(sockfd);
    pthread_exit(0);
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <target_ip> <max_threads>\n", argv[0]);
        return 1;
    }

    char *target_ip = argv[1];
    int max_threads = atoi(argv[2]);

    if (max_threads <= 0) {
        fprintf(stderr, "Invalid number of threads. Must be greater than 0.\n");
        return 1;
    }

    // pthread_t threads[MAX_THREADS];
    pthread_t threads[max_threads];
    // char *target_ip = "8.8.8.8";

    // Spawn threads to perform pinging.
    for (int i = 0; i < max_threads; i++) {
        if (pthread_create(&threads[i], NULL, pingFunction, target_ip)) {
            fprintf(stderr, "Error creating thread\n");
            return 1;
        }
    }

    // Collect metrics and log them.
    while (1) {
        sleep(10);
        pthread_mutex_lock(&metricsMutex);
        printf("Metrics: Successful pings = %u, Failed pings = %u\n", successfulPings, failedPings);
        pthread_mutex_unlock(&metricsMutex);
    }

    // This code will actually never reach here in this example.
    for (int i = 0; i < max_threads; i++) {
        pthread_join(threads[i], NULL);
    }

    return 0;
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