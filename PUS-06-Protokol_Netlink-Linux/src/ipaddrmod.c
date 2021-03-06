#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <linux/netlink.h>
#include <linux/rtnetlink.h>
#include <asm/types.h>
#include <arpa/inet.h>
#include <net/if.h>
#include "libnetlink.h"
/*
 * Program wymaga uprawnien roota.
 */

<<<<<<< HEAD
int main(int argc, char **argv)
{

    int sockfd;            /* Deskryptor gniazda. */
    int retval;            /* Wartosc zwracana przez funkcje. */
    uint32_t addr;         /* Do przechowywania adresu IPv4. */
    unsigned char action;  /* Typ operacji: dodanie/usuniecie adresu IP. */
    struct sockaddr_nl sa; /* Struktura adresowa */
    struct nlmsghdr *nh;   /* Wskaznik na naglowek Netlink. */
    struct ifaddrmsg *ia;  /* Wskaznik na naglowek rodziny. */

    /* Bufor dla wysylanego komunikatu: */
    void *request;
    int request_size;

    if (argc != 5)
    {
        fprintf(
            stderr, "Invocation: %s <INTERFACE NAME> <ACTION>"
                    " <IPv4 ADDRESS> <PREFIX-LENGTH>\n",
            argv[0]);
=======
int main(int argc, char** argv) {

    int                     sockfd; /* Deskryptor gniazda. */
    int                     retval; /* Wartosc zwracana przez funkcje. */
    uint32_t                addr;   /* Do przechowywania adresu IPv4. */
    unsigned char           action; /* Typ operacji: dodanie/usuniecie adresu IP. */
    struct sockaddr_nl      sa;     /* Struktura adresowa */
    struct nlmsghdr         *nh;    /* Wskaznik na naglowek Netlink. */
    struct ifaddrmsg        *ia;    /* Wskaznik na naglowek rodziny. */

    /* Bufor dla wysylanego komunikatu: */
    void                    *request;
    int                     request_size;

    if (argc != 5) {
        fprintf(
            stderr, "Invocation: %s <INTERFACE NAME> <ACTION>"
            " <IPv4 ADDRESS> <PREFIX-LENGTH>\n",
            argv[0]
        );
>>>>>>> 5e237a748256f690f6b3e4a3271043dcb7675d7a

        exit(EXIT_FAILURE);
    }

    /* Okreslenie operacji - dodanie lub usuniecie adresu IPv4: */
<<<<<<< HEAD
    if (strcmp(argv[2], "add") == 0)
    {
        action = RTM_NEWADDR;
    }
    else if (strcmp(argv[2], "del") == 0)
    {
        action = RTM_DELADDR;
    }
    else
    {

        fprintf(
            stderr, "Invalid action! Only \"add\" and \"del\" are allowed.\n");
=======
    if (strcmp(argv[2], "add") == 0) {
        action = RTM_NEWADDR;
    } else if (strcmp(argv[2], "del") == 0) {
        action = RTM_DELADDR;
    } else {

        fprintf(
            stderr, "Invalid action! Only \"add\" and \"del\" are allowed.\n"
        );
>>>>>>> 5e237a748256f690f6b3e4a3271043dcb7675d7a

        exit(EXIT_FAILURE);
    }

    /* Utworzenie gniazda Netlink: */
    sockfd = socket(PF_NETLINK, SOCK_RAW, NETLINK_ROUTE);
<<<<<<< HEAD
    if (sockfd == -1)
    {
=======
    if (sockfd == -1) {
>>>>>>> 5e237a748256f690f6b3e4a3271043dcb7675d7a
        perror("socket()");
        exit(EXIT_FAILURE);
    }

    /*
     * Alokacja pamieci dla wysylanego komunikatu.
     *
     * NLMSG_SPACE(sizeof(struct ifaddrmsg)) = rozmiar naglowka 'nlmsghdr'
     * + opcjonalny padding + rozmiar naglowka 'ifaddrmsg' + opcjonalny padding;
     *
     * RTA_SAPCE(sizeof(uint32_t)) = rozmiar naglowka 'rtattr' + opcjonalny padding
     * + sizeof(uint32_t) + opcjonalny padding;
     *
     * Podobnie RTA_SAPCE(strlen(argv[1])).
     */
<<<<<<< HEAD
    request_size = NLMSG_SPACE(sizeof(struct ifaddrmsg)) + 3 * RTA_SPACE(sizeof(uint32_t)) + RTA_SPACE(strlen(argv[1]));

    request = malloc(request_size);
    if (request == NULL)
    {
=======
    request_size = NLMSG_SPACE(sizeof(struct ifaddrmsg))
                   + 3*RTA_SPACE(sizeof(uint32_t))
                   + RTA_SPACE(strlen(argv[1]));

    request = malloc(request_size);
    if (request == NULL) {
>>>>>>> 5e237a748256f690f6b3e4a3271043dcb7675d7a
        fprintf(stderr, "malloc() failed!\n");
        exit(EXIT_FAILURE);
    }

    /* Wyzerowanie struktury adresowej.
     * Jadro jest odpowiedzialne za ustalenie identyfikatora gniazda.
     * Poniewaz jest to pierwsze (i jedyne) gniazdo procesu,
     * identyfikatorem gniazda bedzie PID procesu. */
    memset(&sa, 0, sizeof(struct sockaddr_nl));
<<<<<<< HEAD
    sa.nl_family = AF_NETLINK;

    if (bind(sockfd, (struct sockaddr *)&sa, sizeof(struct sockaddr_nl)) == -1)
    {
=======
    sa.nl_family            =       AF_NETLINK;
    sa.nl_groups = RTMGRP_LINK | RTMGRP_IPV4_IFADDR;

    if (bind(sockfd, (struct sockaddr*) &sa, sizeof(struct sockaddr_nl)) == -1) {
>>>>>>> 5e237a748256f690f6b3e4a3271043dcb7675d7a
        perror("bind()");
        exit(EXIT_FAILURE);
    }

<<<<<<< HEAD
    nh = (struct nlmsghdr *)request;
=======
    nh                      = (struct nlmsghdr*)request;
>>>>>>> 5e237a748256f690f6b3e4a3271043dcb7675d7a
    /*
     * Rozmiar naglowka 'nlmsghdr' + opcjonalny padding
     * + rozmiar naglowka 'ifaddrmsg':
     */
<<<<<<< HEAD
    nh->nlmsg_len = NLMSG_LENGTH(sizeof(struct ifaddrmsg));
=======
    nh->nlmsg_len           =       NLMSG_LENGTH(sizeof(struct ifaddrmsg));
>>>>>>> 5e237a748256f690f6b3e4a3271043dcb7675d7a
    /*
     * Pole nh->nlmsg_len bedzie aktualizowane
     * przez wywolania funkcji 'addattr_l'.
     */

<<<<<<< HEAD
    nh->nlmsg_type = action; /* RTM_NEWADDR lub RTM_DELADDR */
    nh->nlmsg_flags = NLM_F_REQUEST | NLM_F_ACK;
    nh->nlmsg_seq = 1;
    nh->nlmsg_pid = getpid();

    ia = (struct ifaddrmsg *)NLMSG_DATA(nh);
    ia->ifa_family = AF_INET;
    ia->ifa_prefixlen = atoi(argv[4]);
    ia->ifa_flags = IFA_F_PERMANENT;
    ia->ifa_scope = 0;
    ia->ifa_index = if_nametoindex(argv[1]);

    /* Zero nie jest prawidlowym indeksem interfejsu: */
    if (ia->ifa_index == 0)
    {
=======
    nh->nlmsg_type          =       action; /* RTM_NEWADDR lub RTM_DELADDR */
    nh->nlmsg_flags         =       NLM_F_REQUEST | NLM_F_ACK;
    nh->nlmsg_seq           =       1;
    nh->nlmsg_pid           =       getpid();

    ia                      = (struct ifaddrmsg*)NLMSG_DATA(nh);
    ia->ifa_family          =       AF_INET;
    ia->ifa_prefixlen       =       atoi(argv[4]);
    ia->ifa_flags           =       IFA_F_PERMANENT;
    ia->ifa_scope           =       0;
    ia->ifa_index           =       if_nametoindex(argv[1]);

    /* Zero nie jest prawidlowym indeksem interfejsu: */
    if (ia->ifa_index == 0) {
>>>>>>> 5e237a748256f690f6b3e4a3271043dcb7675d7a
        fprintf(stderr, "Cannot find device \"%s\"!\n", argv[1]);
        exit(EXIT_FAILURE);
    }

    retval = inet_pton(AF_INET, argv[3], &addr);
<<<<<<< HEAD
    if (retval == 0)
    {
        fprintf(stderr, "inet_pton(): invalid IPv4 address!\n");
        exit(EXIT_FAILURE);
    }
    else if (retval == -1)
    {
=======
    if (retval == 0) {
        fprintf(stderr, "inet_pton(): invalid IPv4 address!\n");
        exit(EXIT_FAILURE);
    } else if (retval == -1) {
>>>>>>> 5e237a748256f690f6b3e4a3271043dcb7675d7a
        perror("inet_pton()");
        exit(EXIT_FAILURE);
    }

    addattr_l(nh, request_size, IFA_ADDRESS, (void *)&addr, 4);
    addattr_l(nh, request_size, IFA_LOCAL, (void *)&addr, 4);

<<<<<<< HEAD
    addr = addr & htonl(0xffffffff << (32 - ia->ifa_prefixlen));
    addattr_l(nh, request_size, IFA_BROADCAST, (void *)&addr, 4);
=======
    addr = addr & htonl(0xffffffff<<(32 - ia->ifa_prefixlen));
    addattr_l(nh, request_size, IFA_BROADCAST, (void *) &addr, 4);
>>>>>>> 5e237a748256f690f6b3e4a3271043dcb7675d7a

    /* IFA_LABEL nie moze byc obecne przy usuwaniu.
     * Przy usuwaniu mozna podac eth0 zamiast
     * aliasu eth0:1 (ten sam index interfejsu). */
<<<<<<< HEAD
    if (action == RTM_NEWADDR)
    {
        addattr_l(
            nh, request_size, IFA_LABEL,
            (void *)argv[1], strlen(argv[1]));
=======
    if (action == RTM_NEWADDR) {
        addattr_l(
            nh, request_size, IFA_LABEL,
            (void *) argv[1], strlen(argv[1])
        );
>>>>>>> 5e237a748256f690f6b3e4a3271043dcb7675d7a
    }

    /* Struktura adresowa jest wyzerowana.
     * Wiadomosc bedzie przeznaczona do jadra (sa.nl_pid rowne 0). */
    memset(&sa, 0, sizeof(struct sockaddr_nl));
<<<<<<< HEAD
    sa.nl_family = AF_NETLINK;

    fprintf(stdout, "Sending message to kernel...\n");

    retval = sendto(
        sockfd, request, nh->nlmsg_len, 0,
        (struct sockaddr *)&sa, sizeof(struct sockaddr_nl));

    if (retval == -1)
    {
=======
    sa.nl_family            =       AF_NETLINK;

    fprintf(stdout, "Sending message to kernel...\n");

    nh->nlmsg_pid          =       0;

    retval = sendto(
                 sockfd, request, nh->nlmsg_len, 0,
                 (struct sockaddr*)&sa , sizeof(struct sockaddr_nl)
             );

    if (retval == -1) {
>>>>>>> 5e237a748256f690f6b3e4a3271043dcb7675d7a
        perror("send()");
        exit(EXIT_FAILURE);
    }

<<<<<<< HEAD
    retval = recvfrom(
        sockfd, request, request_size, 0,
        (struct sockaddr *)&sa, sizeof(struct sockaddr_nl));

    if (retval == -1)
    {
        perror("recvfrom()");
        exit(EXIT_FAILURE);
    }

    free(request);
    close(sockfd);
    exit(EXIT_SUCCESS);
}
=======
    // recvfrom kernel 
    // memset(request, 0, request_size);

    // retval = recvfrom(
    //                 sockfd, request, nh->nlmsg_len, 0,
    //                 (struct sockaddr*)&sa , sizeof(struct sockaddr_nl)
    //                 );

    int len;
    char buf[4096];
    struct iovec iov = { buf, sizeof(buf) };
    struct msghdr msg = { &sa, sizeof(sa), &iov, 1, NULL, 0, 0 };

    len = recvmsg(sockfd, &msg, 0);

    if(len <= 0) {
        perror("recvmsg()");
        exit(EXIT_FAILURE);
    }

    for (nh = (struct nlmsghdr *) buf; NLMSG_OK (nh, len); nh = NLMSG_NEXT (nh, len)) {
        if (nh->nlmsg_type == NLMSG_DONE){
            fprintf(stdout, "Done\n");
            return;
        } else if (nh->nlmsg_type == NLMSG_ERROR){
            struct nlmsgerr *err = (struct nlmsgerr *)NLMSG_DATA(nh);
            if (err->error < 0) {
                fprintf(stderr, "Error: %s\n", strerror(-err->error));
                return;
            }
        } else if(nh->nlmsg_type == RTM_NEWADDR) {
            printf("New address created\n");
        } else if(nh->nlmsg_type == RTM_DELADDR) {
            printf("Address deleted\n");
        }
        else{
            fprintf(stdout, "Unknown message type: %d\n", nh->nlmsg_type);
        }
    }


    free(request);
    close(sockfd);
    exit(EXIT_SUCCESS);
}
>>>>>>> 5e237a748256f690f6b3e4a3271043dcb7675d7a
