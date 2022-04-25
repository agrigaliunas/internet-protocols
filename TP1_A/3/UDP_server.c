#include <stdio.h>  
#include <stdlib.h> 
#include <string.h> 
#include <netdb.h>  
#include <netinet/in.h> 
#include <unistd.h> 
#include <sys/types.h>  
#include <sys/socket.h> 
#include <arpa/inet.h>  

#define MYPORT 3490  
#define BACKLOG 5
#define MAXDATASIZE 100

void writeMessage(char message[MAXDATASIZE]){
    fgets(message,MAXDATASIZE,stdin);
}

int main(){
    printf("Servidor iterativo UDP\n");

    int socket_file_descriptor;
    struct sockaddr_in socket_addr;
    struct sockaddr_in client_addr;

    socklen_t addr_size = sizeof(struct sockaddr_in);

    char message_received[MAXDATASIZE];
    char message_to_send[MAXDATASIZE];

    int returned_int;
    int host_addr_size;

    socket_file_descriptor = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if(socket_file_descriptor == -1){
        fprintf(stderr, "No se pudo crear el socket\n");
        exit(1);
    }

    socket_addr.sin_family = AF_INET;
    socket_addr.sin_port   = htons(MYPORT);
    socket_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    bzero(&(socket_addr.sin_zero), 8*sizeof(unsigned char));

    returned_int = bind(socket_file_descriptor, (struct sockaddr *)&socket_addr, sizeof(struct sockaddr));
    if(returned_int == -1){
        fprintf(stderr, "No se pudo bindear\n");
        exit(1);
    }


    while(1){
        memset(message_received, 0, MAXDATASIZE);

        returned_int = recvfrom(socket_file_descriptor, message_received, MAXDATASIZE, 0, (struct sockaddr *)&client_addr, (socklen_t *) &addr_size);
        if(returned_int == -1){
            fprintf(stderr, "No se pudo recibir\n");
            exit(1);
        }

        printf("Mensaje recibido de (%s): %s\n", inet_ntoa(client_addr.sin_addr), message_received);
        
        printf("Escribe tu mensaje: ");
        writeMessage(message_to_send);

        returned_int = sendto(socket_file_descriptor, message_to_send, MAXDATASIZE, 0, (struct sockaddr *)&client_addr, addr_size);
        if(returned_int == -1){
            fprintf(stderr, "No se pudo enviar\n");
            exit(1);
        }
    }

    close(socket_file_descriptor);
    return 0;
}


