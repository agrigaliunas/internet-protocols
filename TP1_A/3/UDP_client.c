#include <stdio.h>  
#include <stdlib.h> 
#include <errno.h>       
#include <string.h> 
#include <netdb.h>  
#include <netinet/in.h> 
#include <unistd.h> 
#include <sys/types.h>  
#include <sys/socket.h> 
#include <sys/wait.h>   
#include <arpa/inet.h>  

#define PORT 3490
#define MAXDATASIZE 100


void writeMessage(char message[MAXDATASIZE]){
    fgets(message,MAXDATASIZE,stdin);
}

int main(int argc, char *argv[]){
    printf("Cliente UDP\n");
    int socket_file_descriptor;
    struct sockaddr_in socket_addr;
    socklen_t socket_addr_len;
    char *message_to_send = argv[2];
    char message_received[MAXDATASIZE];

    int returned_int;
    struct hostent *host_entry;


    if (argc != 3 ){
        fprintf(stderr, "usage: client hostname mensaje\n");
        exit(1);
    }
    
    host_entry = gethostbyname(argv[1]);
    if(host_entry == NULL){
        fprintf(stderr, "No se pudo encontrar el host\n");
        exit(1);
    }

    socket_file_descriptor = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if(socket_file_descriptor == -1){
        fprintf(stderr, "No se pudo crear el socket\n");
        exit(1);
    }

    socket_addr.sin_family = AF_INET;
    socket_addr.sin_port = htons(PORT);
    socket_addr.sin_addr = *((struct in_addr *)host_entry->h_addr);
    bzero(&(socket_addr.sin_zero), 8*sizeof(unsigned char));

    returned_int = connect(socket_file_descriptor, (struct sockaddr *)&socket_addr, sizeof(struct sockaddr));
    if(returned_int == -1){
        fprintf(stderr, "No se pudo conectar\n");
        exit(1);
    }

    returned_int = send(socket_file_descriptor, message_to_send, strlen(message_to_send)*sizeof(char), 0);
    if(returned_int == -1){
        fprintf(stderr, "No se pudo enviar el mensaje\n");
        exit(1);
    }

    printf("Mensaje enviado: %s\n", message_to_send);

    returned_int = recv(socket_file_descriptor, message_received, MAXDATASIZE, 0);

    printf("Message recibido: %s\n", message_received);

    close(socket_file_descriptor);

    return 0;

}


