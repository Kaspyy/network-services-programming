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
#include <sys/select.h>

void print_to_all(int serverSocket, fd_set sockets, char * buff, int excludeSocket);

int main(int argc, char** argv) {

    /* Deskryptory dla gniazda nasluchujacego i polaczonego: */
    int             listenfd, connfd;

    int             retval; /* Wartosc zwracana przez funkcje. */

    /* Gniazdowe struktury adresowe (dla klienta i serwera): */
    struct          sockaddr_in client_addr, server_addr;

    /* Rozmiar struktur w bajtach: */
    socklen_t       client_addr_len, server_addr_len;

    /* Bufor wykorzystywany przez write() i read(): */
    char            buff[256];

    /* Bufor dla adresu IP klienta w postaci kropkowo-dziesietnej: */
    char            addr_buff[256];

    fd_set current_sockets, ready_sockets;

    if (argc != 2) {
        fprintf(stderr, "Invocation: %s <PORT>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    /* Utworzenie gniazda dla protokolu TCP: */
    listenfd = socket(PF_INET, SOCK_STREAM, 0);
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

    /* Przeksztalcenie gniazda w gniazdo nasluchujace: */
    if (listen(listenfd, 4) == -1) {
        perror("listen()");
        exit(EXIT_FAILURE);
    }

    /* Inicjuj gniazda */
    FD_ZERO(&current_sockets);
    FD_SET(listenfd, &current_sockets);

    fprintf(stdout, "Server is listening for incoming connection...\n");

    while(1){
        ready_sockets = current_sockets;
        if(select(FD_SETSIZE, &ready_sockets,NULL,NULL,NULL) < 0){
            perror("select()");
            exit(EXIT_FAILURE);
        }

        for(int i=0; i<FD_SETSIZE; i++){
            if(FD_ISSET(i, &ready_sockets)){
                if(i == listenfd){
                     /* Akceptuj nowe połączenie */
                     connfd = accept(listenfd,(struct sockaddr*)&client_addr, &client_addr_len);
                     if (connfd == -1) {
                        perror("accept()");
                        exit(EXIT_FAILURE);
                    }
                    fprintf(
                        stdout, "TCP connection accepted from %s:%d\n",
                        inet_ntop(AF_INET, &client_addr.sin_addr, addr_buff, sizeof(addr_buff)),
                        ntohs(client_addr.sin_port)
                    );
                    FD_SET(connfd, &current_sockets);
                }
                else {
                    /* Czytaj treść wiadomości */
                    memset(buff,0,sizeof(buff));
                    retval = read(i, buff, sizeof(buff));
                    if (retval == 0 || retval == -1) {
                        fprintf(stdout, "Connection terminated by client "
                                "(received FIN, entering CLOSE_WAIT state on connected socked)...\n");
                        fprintf(stdout, "Closing connected socket (sending FIN to client)...\n");
                        /* Zamknij deskryptor i wyczyść SET*/
                        close(i);
                        FD_CLR(i, &current_sockets);
                    }

                    /* Roześlij wiadomość do wszystkich podłączonych */
                    print_to_all(listenfd, current_sockets, buff, i);
                } 
            }
        }
    }

    fprintf(stdout, "Closing listening socket and terminating server...\n");
    close(listenfd);

    sleep(3);

    exit(EXIT_SUCCESS);
}

void print_to_all(int serverSocket, fd_set sockets, char * buff, int excludeSocket)
{
    for (int i = 0; i < FD_SETSIZE; ++i) {
        if (FD_ISSET(i, &sockets) && i != serverSocket && i != excludeSocket) {
            int retval = write(i, buff, strlen(buff));
            if (retval == -1) {
                perror("write()");
                exit(EXIT_FAILURE);
            }
        }
    }
}