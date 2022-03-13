#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h> /* socket() */
#include <netinet/in.h> /* INET_ADDRSTRLEN, INET6_ADDRSTRLEN */
#include <errno.h>
#include <time.h>
#include <unistd.h>

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

    /* Bufor wykorzystywany przez write() i read() */
    char buff[256];

    if (argc != 2)
    {
        fprintf(stderr, "Invocation: %s <PORT>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    /* Utworzenie gniazda dla protokolu UDP */
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
    if (listen(listenfd, 2) == -1)
    {
        perror("listen()");
        exit(EXIT_FAILURE);
    }

    while (1)
    {
        fprintf(stdout, "Server is listening for incoming connection on port %hu ...\n", server.sin6_port);

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
            ntohs(client.sin_port));

        sleep(6);

        if (IN6_IS_ADDR_V4MAPPED(&client.sin6_addr))
            fprintf(stdout, "Client address is IPv4\n");
        else
            fprintf(stdout, "Client address is IPv6\n");

        retval = write(connfd, )
    }
}
