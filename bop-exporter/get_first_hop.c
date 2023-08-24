#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>

int 
main() {
    char line[100];
    char *gateway_ip = NULL;

    // Execute "route" command and get the result in pipe
    FILE *fp = popen("route -n | grep 'UG[ \t]' | awk '{print $2}'", "r");
    if (fp == NULL) {
        perror("popen");
        exit(EXIT_FAILURE);
    }

    // Read the gateway IP from the result
    while (fgets(line, sizeof(line), fp) != NULL) {
        line[strcspn(line, "\n")] = 0; // Remove newline character
        gateway_ip = strdup(line);
    }
    pclose(fp);

    if (gateway_ip != NULL) {
        printf("Default gateway\n");
        printf("---------------\n");
        printf("IP: %s\n", gateway_ip);
        // free(gateway_ip);
    } else {
        printf("IP: not found\n");
    }

    // typecast the gateway_ip char pointer to char array to avoid warning
    // char gateway_ip_array[50];
    // strcpy(gateway_ip_array, gateway_ip);

    // Construct a command to get the MAC address from the ARP table
    char command[100];
    snprintf(command, sizeof(command), "grep '%s[ \t]' /proc/net/arp | awk '{print $4}'", gateway_ip);
    // printf("Command: %s\n", command);

    FILE *arp_fp = popen(command, "r");
    char mac_address[50];
    if (fgets(mac_address, sizeof(mac_address), arp_fp) != NULL) {
        // Remove any trailing newline
        mac_address[strcspn(mac_address, "\n")] = 0;
        printf("MAC: %s\n", mac_address);
    } else {
        printf("MAC: not found\n");
    }
    pclose(arp_fp);
}