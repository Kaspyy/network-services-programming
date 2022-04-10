#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/sctp.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <netdb.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

#define BUFF_SIZE 256

int main(int argc, char **argv)
{
    int sockfd;
    int retval, flags, slen;
    struct addrinfo hints, *result;
    char buff[BUFF_SIZE];
    struct sctp_initmsg initmsg;
    struct sctp_sndrcvinfo sndrcvinfo;
    struct sctp_event_subscribe events;
    struct sctp_status status;

    if (argc != 3)
    {
        fprintf(stderr, "Invocation: %s <IP ADDRESS> <PORT NUMBER>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_UNSPEC; /* Allow IPv4 or IPv6 */
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = 0;
    hints.ai_protocol = 0;

    retval = getaddrinfo(argv[1], argv[2], &hints, &result);

    if (retval != 0)
    {
        fprintf(stderr, "getaddrinfo(): %s\n", gai_strerror(retval));
        exit(EXIT_FAILURE);
    }

    if (result == NULL)
    {
        fprintf(stderr, "Could not connect!\n");
        exit(EXIT_FAILURE);
    }

    /* Tworzenie gniazda typu one-to-one */
    sockfd = socket(result->ai_family, result->ai_socktype, IPPROTO_SCTP);

    if (sockfd == -1)
    {
        perror("socket()");
        exit(EXIT_FAILURE);
    }

    memset(&initmsg, 0, sizeof(initmsg));
    /* Określenie liczby strumieni wychodzących */
    initmsg.sinit_num_ostreams = 3;
    /* Określenie liczby strumieni przychodzących */
    initmsg.sinit_max_instreams = 4;
    /* Określenie liczby prób nawiązania asocjacji */
    initmsg.sinit_max_attempts = 5;
    retval = setsockopt(sockfd, IPPROTO_SCTP, SCTP_INITMSG, &initmsg, sizeof(initmsg));
    if (retval != 0)

    {
        perror("setsockopt()");
        exit(EXIT_FAILURE);
    }
    if (connect(sockfd, result->ai_addr, result->ai_addrlen) == -1)
    {
        perror("connect()");
        exit(EXIT_FAILURE);
    }

    freeaddrinfo(result);

    memset(&events, 0, sizeof(events));
    events.sctp_data_io_event = 1;
    /* setsockopt() - ustawienie opcji dla gniazda */
    retval = setsockopt(sockfd, SOL_SCTP, SCTP_EVENTS, (const void *)&events, sizeof(events));

    slen = sizeof(status);
    /* getsockopt() - pobranie opcji dla gniazda */
    retval = getsockopt(sockfd, SOL_SCTP, SCTP_STATUS, &status, (socklen_t *)&slen);

    /* Wypisanie informacji o asocjacji */
    printf("ID: %d\n", status.sstat_assoc_id);
    printf("STATE: %d\n", status.sstat_state);
    printf("INSTREAMS: %d\n", status.sstat_instrms);
    printf("OUTSTREAMS: %d\n", status.sstat_outstrms);

    /* Odbieranie wiadomości */
    for (int i = 0; i < initmsg.sinit_num_ostreams - 1; i++)
    {
        memset(buff, 0, sizeof(buff));
        /* sctp_recvmsg() odbiera wiadomość z gniazda */
        retval = sctp_recvmsg(sockfd, buff, BUFF_SIZE, (struct sockaddr *)NULL, 0, &sndrcvinfo, &flags);

        if (retval > 0)
        {
            buff[retval] = 0;
            printf("(STREAM: %d) %s\n", sndrcvinfo.sinfo_stream, buff);
            fflush(stdout);
        }
    }
    close(sockfd);
    exit(EXIT_SUCCESS);
}