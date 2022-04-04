#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#define PORT 3490
#define MAXDATASIZE 100


int main(int argc, char *argv[]){ // argc

    int socket_file_descriptor;
    int bytes_recieved;
    char messageReceived[MAXDATASIZE];

    struct hostent *he; // contiene campos con informacion del host
    struct sockaddr_in their_addr; // guarda info sobre quien se va a conectar

    if(argc != 2){
        fprintf(stderr,"Por favor ingrese el hostname del cliente\n");
        exit(1);
    }

    // completo la estructura "he" y reviso si esta NULL

    if ((he=gethostbyname(argv[1])) == NULL){ // gethostbyname retorna puntero a estructura hostent
        perror("gethostbyname");
        exit(1);
    }

    // AF_INET -> familia de direcciones
    // SOCK_STREAM -> para decir que es un socket de flujo. TCP/IP
    if ((socket_file_descriptor = socket(AF_INET, SOCK_STREAM, 0)) == -1){
        perror("socket");
        exit(1);
    }

    their_addr.sin_family = AF_INET;
    their_addr.sin_port = htons(PORT);
    their_addr.sin_addr = *((struct in_addr *)he->h_addr);
    bzero(&(their_addr.sin_zero), 8);


    if (connect(socket_file_descriptor, (struct sockaddr *)&their_addr, sizeof(struct sockaddr)) == -1){
        perror("connect");
        exit(1);
    }

    if ((bytes_recieved=recv(socket_file_descriptor, messageReceived, MAXDATASIZE, 0)) == -1){
        perror("recv");
        exit(1);
    }
    while(strcmp(messageReceived, "disc") != 0 && strlen(messageReceived) != 0){

        messageReceived[bytes_recieved] = '\0';

        printf("Mensaje recibido del server: %s\n", messageReceived);
        printf("Bytes recibidos: %ld\n",strlen(messageReceived));

        if ((bytes_recieved=recv(socket_file_descriptor, messageReceived, MAXDATASIZE, 0)) == -1){
            perror("recv");
            exit(1);
        }

    }

    close(socket_file_descriptor);

    return 0;

}