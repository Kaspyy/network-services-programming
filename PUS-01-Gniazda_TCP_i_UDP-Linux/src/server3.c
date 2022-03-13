/*
 * Data:                2009-02-10
 * Autor:               Jakub Gasior <quebes@mars.iti.pk.edu.pl>
 * Kompilacja:          $ gcc server1.c -o server1
 * Uruchamianie:        $ ./server1 <numer portu>
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h> /* socket() */
#include <netinet/in.h> /* struct sockaddr_in */
#include <arpa/inet.h>  /* inet_ntop() */
#include <unistd.h>     /* close() */
#include <string.h>
#include <time.h>
#include <errno.h>
#include "libpalindrome.h"

int main(int argc, char** argv) {

    /* Deskryptory dla gniazda nasluchujacego i polaczonego: */
    int             listenfd;

    int             retval; /* Wartosc zwracana przez funkcje. */

    /* Gniazdowe struktury adresowe (dla klienta i serwera): */
    struct          sockaddr_in client_addr, server_addr;

    /* Rozmiar struktur w bajtach: */
    socklen_t       client_addr_len, server_addr_len;

    /* Bufor wykorzystywany przez write() i read(): */
    char            buff[256];

    /* Bufor dla adresu IP klienta w postaci kropkowo-dziesietnej: */
    char            addr_buff[256];


    if (argc != 2) {
        fprintf(stderr, "Invocation: %s <PORT>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    /* Utworzenie gniazda dla protokolu UDP: */
    listenfd = socket(PF_INET, SOCK_DGRAM, 0);
    if (listenfd == -1) {
        perror("socket()");
        exit(EXIT_FAILURE);
    }

    /* Wyzerowanie struktury adresowej serwera: */
    memset(&server_addr, 0, sizeof(server_addr));
    /* Domena komunikacyjna (rodzina protokolow): */
    server_addr.sin_family          =       AF_INET;
    /* Adres nieokreslony (ang. wildcard address): */
    server_addr.sin_addr.s_addr     =       htonl(INADDR_ANY);
    /* Numer portu: */
    server_addr.sin_port            =       htons(atoi(argv[1]));

    /* Rozmiar struktury adresowej serwera w bajtach: */
    server_addr_len                 =       sizeof(server_addr);

    /* Powiazanie "nazwy" (adresu IP i numeru portu) z gniazdem: */
    if (bind(listenfd, (struct sockaddr*) &server_addr, server_addr_len) == -1) {
        perror("bind()");
        exit(EXIT_FAILURE);
    }

    fprintf(stdout, "Server is listening for incoming connection...\n");

    sleep(2);

    while(1){
        /* Funkcja oczekuje na datagram (od klienta): */
        fprintf(stdout, "Waiting for data...\n");
        retval = recvfrom(listenfd, buff, sizeof(buff),0,(struct sockaddr*)&client_addr, &client_addr_len);
        if (retval == -1) {
            perror("recvfrom()");
            exit(EXIT_FAILURE);
        }
        sleep(2);

        if(buff[0]=='\n') break;

        fprintf(stdout, "UDP datagram received from %s:%d. Echoing message...\n",
            inet_ntop(AF_INET, &client_addr.sin_addr, addr_buff, sizeof(addr_buff)),
            ntohs(client_addr.sin_port)
        );
        
        /* Sprawdzenie czy treść requesta jest palindromem */
        if(is_palindrome(buff,strlen(buff)) == 1)
            sprintf(buff,"Jest palindromem liczbowym\n");
        else    
            sprintf(buff,"Nie jest palindromem\n");
        

        /* Wyslanie odpowiedzi do klienta: */
        retval = sendto(
                 listenfd,
                 buff, retval,
                 0,
                 (struct sockaddr*)&client_addr, client_addr_len
             );
        if (retval == -1) {
            perror("sendto()");
            exit(EXIT_FAILURE);
        }
    }

    sleep(5);

    fprintf(stdout, "Closing listening socket and terminating server...\n");
    close(listenfd);

    sleep(3);

    exit(EXIT_SUCCESS);
}
