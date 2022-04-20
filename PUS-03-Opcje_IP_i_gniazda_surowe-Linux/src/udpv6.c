#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/udp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>
#include <netdb.h>
#include "checksum.h"

int main(int argc, char **argv)
{

    int sockfd;
    int socket_option;
    int retval;

    /* struktura phdr - pseudo-naglowek */
    struct phdr
    {
        struct in_addr ip_src, ip_dst;
        unsigned char unused;
        unsigned char protocol;
        unsigned short length;
    };

    unsigned short checksum;
    unsigned char datagram[sizeof(struct udphdr) + sizeof(struct phdr)] = {0};

    struct udphdr *udp_header = (struct udphdr *)(datagram);
    struct phdr *pseudo_header = (struct phdr *)(datagram + sizeof(struct udphdr));
    struct addrinfo hints;
    struct addrinfo *rp, *result;

    if (argc != 3)
    {
        fprintf(stderr, "Usage: %s <IPv6 address> <port number>\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    /* hints - struktura zawierajaca wskazowki dla funkcji getaddrinfo() */
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_INET6;
    hints.ai_socktype = SOCK_RAW;
    hints.ai_protocol = IPPROTO_UDP;

    retval = getaddrinfo(argv[1], NULL, &hints, &result);
    if (retval != 0)
    {
        fprintf(stderr, "getaddrinfo(): %s\n", gai_strerror(retval));
        exit(EXIT_FAILURE);
    }

    socket_option = 1;

    for (rp = result; rp != NULL; rp = rp->ai_next)
    {
        sockfd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);

        if (sockfd == -1)
        {
            printf("Socket error\n\terrno: %d\n", errno);
            perror("");
            continue;
        }
        /* położenie sumy kontrolnej w nagłówku UDP */
        int offset = 6;

        /* setsockopt - ustawia opcje gniazda */
        retval = setsockopt(sockfd, IPPROTO_IPV6, IPV6_CHECKSUM, &offset, sizeof(offset));

        if (retval == -1)
        {
            printf("Setsocketopt error\n\terrno: %d\n", errno);
            perror("");
            exit(EXIT_FAILURE);
        }
        else
        {
            printf("Socket created\n");
            break;
        }
    }

    if (rp == NULL)
    {
        printf("Socket error\n\terrno: %d\n", errno);
        perror("");
        exit(EXIT_FAILURE);
    }

    /* Wypelnienie naglowka UDP */
    /*uh_dport - port docelowy */
    udp_header->uh_dport = htons(atoi(argv[2]));
    /*uh_len - dlugosc naglowka UDP */
    udp_header->uh_ulen = htons(sizeof(struct udphdr));
    /* Wypełnienie pseudo-nagłówka */
    pseudo_header->ip_dst.s_addr = ((struct sockaddr_in *)rp->ai_addr)->sin_addr.s_addr;
    pseudo_header->unused = 0;
    /* protocol - protokół */
    pseudo_header->protocol = IPPROTO_UDP;
    pseudo_header->length = udp_header->uh_ulen;
    /* uh_sum - suma kontrolna */
    udp_header->uh_sum = 0;
    /* internet_checksum - suma kontrolna */
    checksum = internet_checksum((unsigned short *)udp_header, sizeof(struct udphdr) + sizeof(struct phdr));

    if (checksum == 0)
        udp_header->uh_sum = 0xffff;
    else
        udp_header->uh_sum = checksum;

    printf("Sending empty packet...\n");

    for (;;)
    {
        retval = sendto(sockfd, datagram, sizeof(struct udphdr), 0, rp->ai_addr, rp->ai_addrlen);

        if (retval == -1)
        {
            printf("Sendto error\n\terrno: %d\n", errno);
            perror("");
        }
        else
            printf("Packet sent\n");

        sleep(1);
    }

    exit(EXIT_SUCCESS);
}
