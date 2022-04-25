#include <stdio.h>  
#include <stdlib.h> 
#include <errno.h>  
#include <string.h> 
#include <netdb.h>  
#include <netinet/in.h> 
#include <unistd.h> 
#include <sys/types.h> 
#include <sys/socket.h> 

#define PORT 3490
#define MAXDATASIZE 100

void writeMessage(char message[MAXDATASIZE]){
    fgets(message,MAXDATASIZE,stdin);
}

int main(int argc, char *argv[]){ 

    int socket_file_descriptor;
    struct hostent *he; 
    struct sockaddr_in socket_addr; 
    int bytes_received;
    int returned_int;
    char message_received[MAXDATASIZE];
    char message_to_send[MAXDATASIZE];


    if(argc != 2){
        fprintf(stderr,"Usage client hostname\n");
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

    socket_addr.sin_family = AF_INET;
    socket_addr.sin_port = htons(PORT);
    socket_addr.sin_addr = *((struct in_addr *)he->h_addr);
    bzero(&(socket_addr.sin_zero), 8*sizeof(unsigned char));

    returned_int = connect(socket_file_descriptor, (struct sockaddr *)&socket_addr, sizeof(struct sockaddr));
    if (returned_int == -1){
        perror("connect");
        exit(1);
    }

    while(strcmp(message_to_send, "disc") != 0 || strcmp(message_received, "disc") != 0){

        printf("Escribe el mensaje: ");
        writeMessage(message_to_send);

        returned_int = send(socket_file_descriptor, message_to_send, strlen(message_to_send)*sizeof(char), 0);
        if(returned_int == -1){
            perror("send");
            exit(1);
        }
        printf("Mensaje enviado\n");

        if ((bytes_received=recv(socket_file_descriptor, message_received, MAXDATASIZE, 0)) == -1){
            perror("recv");
            exit(1);
        }
        message_received[bytes_received] = '\0';

        printf("Server: %s\n", message_received);

    }

    close(socket_file_descriptor);

    return 0;

}