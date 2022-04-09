#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <arpa/inet.h> /* inet_pton() */
#include <net/if_arp.h>
#include <netinet/in.h> /* struct sockaddr_in */
#include <sys/ioctl.h>
#include <net/if.h>

struct ifreq ifr;
int sockfd;

void print_interface(char* name){
    memset(&ifr, 0, sizeof(struct ifreq));
    strcpy(ifr.ifr_name, name);
    printf("Name: %s\n", ifr.ifr_name);

    if (ioctl(sockfd, SIOCGIFMTU, &ifr) == -1) {
        perror("ioctl()");
        exit(EXIT_FAILURE);
    }
    printf ("MTU: %d\n", ifr.ifr_mtu);

    if (ioctl(sockfd, SIOCGIFHWADDR, &ifr) == -1) {
        perror("ioctl()");
        exit(EXIT_FAILURE);
    }
    printf ("MAC: %02x:%02x:%02x:%02x:%02x:%02x\n",
    (unsigned char)ifr.ifr_hwaddr.sa_data[0],
    (unsigned char)ifr.ifr_hwaddr.sa_data[1],
    (unsigned char)ifr.ifr_hwaddr.sa_data[2],
    (unsigned char)ifr.ifr_hwaddr.sa_data[3],
    (unsigned char)ifr.ifr_hwaddr.sa_data[4],
    (unsigned char)ifr.ifr_hwaddr.sa_data[5]);
}

int main(int argc, char** argv) {
    
    if (argc != 4) {
        fprintf(stderr, "Invocation: %s <interface name> <mac address> <mtu>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd == -1) {
        perror("socket()");
        exit(EXIT_FAILURE);
    }

    print_interface(argv[1]);

    // Set interface MTU
    ifr.ifr_mtu = atoi(argv[3]);
    
    if (ioctl(sockfd, SIOCSIFMTU, &ifr) == -1) {
        perror("ioctl()");
        exit(EXIT_FAILURE);
    }

    //Set interface MAC address
    ifr.ifr_hwaddr.sa_family = ARPHRD_ETHER;
    sscanf(
        argv[2], "%2x:%2x:%2x:%2x:%2x:%2x",
        (unsigned int*)&ifr.ifr_hwaddr.sa_data[0],
        (unsigned int*)&ifr.ifr_hwaddr.sa_data[1],
        (unsigned int*)&ifr.ifr_hwaddr.sa_data[2],
        (unsigned int*)&ifr.ifr_hwaddr.sa_data[3],
        (unsigned int*)&ifr.ifr_hwaddr.sa_data[4],
        (unsigned int*)&ifr.ifr_hwaddr.sa_data[5]
    );    

    if (ioctl(sockfd, SIOCSIFHWADDR, &ifr) == -1) {
        perror("ioctl()");
        exit(EXIT_FAILURE);
    }

    printf("\nNew\n");
    print_interface(argv[1]);

    // Close socket
    if (close(sockfd) == -1) {
        perror("close()");
        exit(EXIT_FAILURE);
    }

    return 0;
}