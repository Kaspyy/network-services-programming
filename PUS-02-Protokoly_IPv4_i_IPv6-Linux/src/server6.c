#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h> /* socket() */
#include <netinet/in.h> /* struct sockaddr_in */
#include <arpa/inet.h>  /* inet_ntop() */
#include <unistd.h>     /* close() */
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/select.h> // select, poll
#include <sys/types.h>
#include <netinet/in.h> // htons, htonl, ntohs, ntohl
#include <arpa/inet.h>  // inet_pton, inet_ntop

int main(int argc, char **argv)
{
    /* Deskryptory dla gniazda nasluchujacego i polaczonego: */
    int listenfd, connfd;
    int sockfd; /* Deskryptor gniazda */
    int retval; /* Wartosc zwracana przez funkcje */

    /*Rozmiar struktur w bajtach */
    socklen_t server_len, client_len;

    struct sockaddr_in6 server, client; /* Struktura adresowa dla IPv6 (serwer, klient)*/

    /* Bufor dla adresu IP klienta w postaci kropkowo-dziesietnej: */
    char addr_buff[256];

    if (argc != 2)
    {
        fprintf(stderr, "Invocation: %s <PORT>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    /* Utworzenie gniazda dla serwera */
    listenfd = socket(PF_INET6, SOCK_STREAM, 0);
    if (listenfd == -1)
    {
        perror("socket()");
        exit(EXIT_FAILURE);
    }
    /* Wyzerowanie struktury adresowej serwera: */
    memset(&server, 0, sizeof(server));
    /* Domena komunikacyjna (rodzina protokolow): */
    server.sin6_family = AF_INET6;
    /* Adres nieokreslony (ang. wildcard address): */
    server.sin6_addr = in6addr_any;
    /* Numer portu: */
    server.sin6_port = htons(atoi(argv[1]));

    /* Rozmiar struktury adresowej serwera w bajtach: */
    server_len = sizeof(server);

    /* Powiazanie "nazwy" (adresu IP i numeru portu) z gniazdem: */
    if (bind(listenfd, (struct sockaddr *)&server, server_len) == -1)
    {
        perror("bind()");
        exit(EXIT_FAILURE);
    }

    /* Przeksztalcenie gniazda w gniazdo nasluchujace: */
    if (listen(listenfd, 100) == -1)
    {
        perror("listen()");
        exit(EXIT_FAILURE);
    }

    char *message = "Laboratorium PUS";
    while (1)
    {
        fprintf(stdout, "Server is listening for incoming connection on port %hu ...\n", ntohs(server.sin6_port));

        /* Funkcja pobiera polaczenie z kolejki polaczen oczekujacych na zaakceptowanie i zwraca deskryptor dla gniazda polaczonego */
        client_len = sizeof(client);
        connfd = accept(listenfd, (struct sockaddr *)&client, &client_len);
        if (connfd == -1)
        {
            perror("accept()");
            exit(EXIT_FAILURE);
        }

        fprintf(
            stdout, "TCP connection accepted from %s: %d\n",
            inet_ntop(AF_INET6, &client.sin6_addr, addr_buff, sizeof(addr_buff)),
            ntohs(client.sin6_port));

        /* Makrodefinicja IN6_IS_ADDR_V4MAPPED sprawdza czy
        adres zwrocoony przez funkcje accept() jest adresem IPv6 czy IPv4-mapped IPv6 */
        if (IN6_IS_ADDR_V4MAPPED(&client.sin6_addr))
            fprintf(stdout, "Client address is IPv4\n");
        else
            fprintf(stdout, "Client address is IPv6\n");

        retval = send(connfd, message, strlen(message), 0);
        if (retval == -1)
        {
            perror("send()");
            exit(EXIT_FAILURE);
        }
    }
    exit(EXIT_SUCCESS);
}
