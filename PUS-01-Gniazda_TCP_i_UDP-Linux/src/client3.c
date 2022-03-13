/*
 * Data:                2009-02-10
 * Autor:               Jakub Gasior <quebes@mars.iti.pk.edu.pl>
 * Kompilacja:          $ gcc client1.c -o client1
 * Uruchamianie:        $ ./client1 <adres IP> <numer portu>
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h> /* socket() */
#include <netinet/in.h> /* struct sockaddr_in */
#include <arpa/inet.h>  /* inet_pton() */
#include <unistd.h>     /* close() */
#include <string.h>
#include <errno.h>

int main(int argc, char** argv) {

    int             sockfd;                 /* Desktryptor gniazda. */
    int             retval;                 /* Wartosc zwracana przez funkcje. */
    struct          sockaddr_in remote_addr;/* Gniazdowa struktura adresowa. */
    socklen_t       addr_len;               /* Rozmiar struktury w bajtach. */
    char            buff[256];              /* Bufor dla funkcji read(). */

    if (argc != 3) {
        fprintf(stderr, "Invocation: %s <IPv4 ADDRESS> <PORT>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    /* Utworzenie gniazda dla protokolu TCP: */
    sockfd = socket(PF_INET, SOCK_DGRAM, 0);
    if (sockfd == -1) {
        perror("socket()");
        exit(EXIT_FAILURE);
    }

    /* Wyzerowanie struktury adresowej dla adresu zdalnego (serwera): */
    memset(&remote_addr, 0, sizeof(remote_addr));

    /* Domena komunikacyjna (rodzina protokolow): */
    remote_addr.sin_family = AF_INET;

    /* Konwersja adresu IP z postaci czytelnej dla czlowieka: */
    retval = inet_pton(AF_INET, argv[1], &remote_addr.sin_addr);
    if (retval == 0) {
        fprintf(stderr, "inet_pton(): invalid network address!\n");
        exit(EXIT_FAILURE);
    } else if (retval == -1) {
        perror("inet_pton()");
        exit(EXIT_FAILURE);
    }

    remote_addr.sin_port = htons(atoi(argv[2])); /* Numer portu. */
    addr_len = sizeof(remote_addr); /* Rozmiar struktury adresowej w bajtach. */

    /* Nawiazanie polaczenia (utworzenie asocjacji,
     * skojarzenie adresu zdalnego z gniazdem): */
    if (connect(sockfd, (const struct sockaddr*) &remote_addr, addr_len) == -1) {
        perror("connect()");
        exit(EXIT_FAILURE);
    }

    while(1){
    memset(buff, 0, 256);
    /* Pobieranie danych z wejścia */
    printf("\nInput message: ");
    fgets(buff,sizeof(buff),stdin);

    /* Wysłanie danych: */
    retval = send(sockfd,buff,sizeof(buff),0);
    if (retval == -1) {
        perror("recv()");
        exit(EXIT_FAILURE);
    }
    sleep(1);
    fprintf(stdout, "Sent server request: %s", buff);

    if(buff[0]=='\n') break;

    memset(buff, 0, 256);
    /* Odebranie danych: */
    retval = recv(sockfd,buff,sizeof(buff),0);
    if (retval == -1) {
        perror("recv()");
        exit(EXIT_FAILURE);
    }
    sleep(1);
    fprintf(stdout, "Received server response: %s", buff);

    sleep(4);
    }

    /* Zamkniecie polaczenia UDP: */
    fprintf(stdout, "Closing socket (sending FIN to server)...\n");
    close(sockfd);

    sleep(9);

    /* Po zakonczeniu aplikacji, gniazdo przez okreslony czas (2 * MSL) bedzie
     * w stanie TIME_WAIT: */
    fprintf(stdout, "Terminating application. After receiving FIN from server, "
            "UDP connection will go into TIME_WAIT state.\n");

    sleep(4);

    exit(EXIT_SUCCESS);
}
