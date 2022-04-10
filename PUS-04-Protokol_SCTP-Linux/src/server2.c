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
#include <time.h>

#define BUFF_SIZE 256

int main(int argc, char **argv)
{
    int listenfd, connfd;
    int retval;
    struct sockaddr_in servaddr;
    char buffer[BUFF_SIZE];
    struct sctp_initmsg initmsg;

    if (argc != 2)
    {
        fprintf(stderr, "Invocation: %s <PORT NUMBER>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    listenfd = socket(PF_INET, SOCK_STREAM, IPPROTO_SCTP);
    if (listenfd == -1)
    {
        perror("socket()");
        exit(EXIT_FAILURE);
    }
    /* Ustawianie liczby strumieni przychodzących i wychodzących */
    memset(&initmsg, 0, sizeof(initmsg));
    initmsg.sinit_num_ostreams = 2;
    initmsg.sinit_max_instreams = 2;
    retval = setsockopt(listenfd, IPPROTO_SCTP, SCTP_INITMSG, &initmsg, sizeof(initmsg));
    if (retval == -1)
    {
        perror("setsockopt()");
        exit(EXIT_FAILURE);
    }

    /* Ustalanie adresu IP i numeru portu */
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(atoi(argv[1]));
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(listenfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) == -1)
    {
        perror("bind()");
        exit(EXIT_FAILURE);
    }
    if (listen(listenfd, 5) == -1)
    {
        perror("listen()");
        exit(EXIT_FAILURE);
    }

    /* Pętla do obsługi połączeń */
    while (1)
    {
        connfd = accept(listenfd, NULL, 0);
        if (connfd == -1)
        {
            perror("accept()");
            exit(EXIT_FAILURE);
        }

        /* Pobieranie daty i czasu */
        time_t t = time(NULL);
        struct tm *tm = localtime(&t);

        /* Wysłanie daty  */
        memset(buffer, 0, sizeof(buffer));
        sprintf(buffer, "%d-%d-%d", tm->tm_year + 1900, tm->tm_mon + 1, tm->tm_mday);
        retval = sctp_sendmsg(connfd, buffer, strlen(buffer), NULL, 0, 0, 0, 0, 0, 0);
        if (retval == -1)
        {
            perror("sctp_sendmsg()");
            exit(EXIT_FAILURE);
        }

        /* Wysyłanie czasu */
        memset(buffer, 0, sizeof(buffer));
        sprintf(buffer, "%d:%d:%d", tm->tm_hour, tm->tm_min, tm->tm_sec);
        retval = sctp_sendmsg(connfd, buffer, strlen(buffer), NULL, 0, 0, 0, 1, 0, 0);
        if (retval == -1)
        {
            perror("sctp_sendmsg()");
            exit(EXIT_FAILURE);
        }
    }
}
