#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <arpa/inet.h>

#define MYPORT 3490  // Puerto al cual nos conectaremos

#define BACKLOG 5 // Cantidad de conecciones en la cola.
#define MAXDATASIZE 100

void writeMessage(char message[MAXDATASIZE]){
    printf("Write your message\n ");
    fgets(message,MAXDATASIZE,stdin);
}


int main(){
    int socket_file_descriptor;
    int new_file_descriptor; // escuchamos en sockfd, nueva coneccion en new_fd
    struct sockaddr_in my_addr; 
    struct sockaddr_in their_addr;
    socklen_t sin_size;
    int bytes_recieved;
    char messageReceived[MAXDATASIZE];
    char messageToSend[MAXDATASIZE];


    if ((socket_file_descriptor = socket(AF_INET, SOCK_STREAM, 0)) == -1){
        perror("socket");
        exit(1);
    }

    my_addr.sin_family = AF_INET;
    my_addr.sin_port   = htons(MYPORT);
    my_addr.sin_addr.s_addr = INADDR_ANY;
    bzero(&(my_addr.sin_zero), 8);

    if (bind(socket_file_descriptor, (struct sockaddr *)&my_addr, sizeof(struct sockaddr)) == -1){
        perror("bind");
        exit(1);
    }

    if (listen(socket_file_descriptor, BACKLOG) == - 1){
        perror("listen");
        exit(1);
    }

    while(1){
        sin_size = sizeof(struct sockaddr_in);
        
        if ((new_file_descriptor = accept(socket_file_descriptor, (struct sockaddr *)&their_addr, &sin_size)) == -1){
            perror("accept");
            continue;
        }
        
        printf("server: conexion entrante de: %s\n", inet_ntoa(their_addr.sin_addr));

        if(!fork()){
            while(strcmp(messageToSend, "disc") != 0 || strcmp(messageReceived, "disc") != 0){

                if ((bytes_recieved=recv(new_file_descriptor, messageReceived, MAXDATASIZE, 0)) == -1){
                    perror("recv");
                    exit(1);
                }
                messageReceived[bytes_recieved] = '\0';

                printf("Cliente: %s\n", messageReceived);

                writeMessage(messageToSend);

                if (send(new_file_descriptor, messageToSend, strlen(messageToSend), 0) == -1){
                perror("send");
                }

                else printf("Mensaje enviado\n");
            }
        }  
        
        else close(new_file_descriptor);

        while(waitpid(-1, NULL, WNOHANG) > 0);
        
    }

    return 0;
}
