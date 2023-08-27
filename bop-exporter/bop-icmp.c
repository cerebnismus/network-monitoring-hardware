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