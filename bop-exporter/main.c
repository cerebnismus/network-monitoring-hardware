#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/ip_icmp.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/time.h>

// gcc -shared -o icmp_ping.so -fPIC icmp_ping.c

#define TARGET "8.8.8.8" // Add your target IP

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

int main() {
    struct icmphdr icmp_hdr;
    struct sockaddr_in addr;
    int sockfd, ret;
    char packet[8];
    struct timeval start, end;

    sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_ICMP);
    if (sockfd < 0) {
        perror("socket");
        return EXIT_FAILURE;
    }

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
    // Handle response, calculate latency, and expose metrics here
    gettimeofday(&end, NULL);
    long latency = (end.tv_sec - start.tv_sec) * 1000 + (end.tv_usec - start.tv_usec) / 1000;
    printf("Latency: %ld ms\n", latency);

    close(sockfd);
    return EXIT_SUCCESS;
}
