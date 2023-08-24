#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <net/if.h>
#include <unistd.h>

int main() {
    struct ifreq ifr;
    int fd = socket(AF_INET, SOCK_DGRAM, 0);

    // Provide the name of the network interface you're interested in
    strcpy(ifr.ifr_name, "eth0");

    // Fetch the MAC address
    if (ioctl(fd, SIOCGIFHWADDR, &ifr) != -1) {
        unsigned char *mac = (unsigned char *)ifr.ifr_hwaddr.sa_data;
        printf("MAC Address: %02x:%02x:%02x:%02x:%02x:%02x\n",
               mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
    } else {
        perror("ioctl");
    }

    close(fd);
    return 0;
}
