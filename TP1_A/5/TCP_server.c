#include <stdio.h>
#include <stdlib.h> 
#include <string.h> 
#include <netdb.h>  
#include <netinet/in.h> 
#include <unistd.h> 
#include <sys/types.h>  
#include <sys/socket.h> 
#include <sys/wait.h>   
#include <arpa/inet.h>  
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/stat.h>

#define MYPORT 3490  
#define BACKLOG 5 
#define MAXDATASIZE 100
#define MAXMESSAGES 1024
#define MAXALIASSIZE 25

typedef char shm_message[MAXDATASIZE];

struct shm_register{
    char alias[MAXALIASSIZE];
    shm_message message;
};

struct shm_segment{
    int first, last, count;
    struct shm_register registers[MAXMESSAGES];
};

void writeMessage(char message[MAXDATASIZE]){
    fgets(message,MAXDATASIZE,stdin);
}

void shm_message_reg(struct shm_segment * address , char message[MAXDATASIZE], char alias[MAXALIASSIZE]){
        if(address->count < MAXMESSAGES){
            address->first = (address->first+1) % MAXMESSAGES;
            strcpy(((address->registers[address->first]).message), message);
            strcpy(((address->registers[address->first]).alias), alias);
            address->count++;
        }

}

int main(){
    int socket_file_descriptor;
    int new_client_file_descriptor; 
    struct sockaddr_in sv_socket_addr; 
    struct sockaddr_in new_client_socket_addr;
    socklen_t socket_addr_size;
    char message_received[MAXDATASIZE];
    char message_to_send[MAXDATASIZE];
    int returned_int;
    int process_id;
    int bytes_received;

    int segment_id = 0;
    key_t shm_key = 0;
    struct shm_segment * shm_addr = NULL;


    shm_key = ftok("sniffer.c", (int) 'A');
    segment_id = shmget(shm_key, (size_t) sizeof(struct shm_segment), IPC_CREAT | IPC_EXCL | S_IRUSR | S_IWUSR );
    if(segment_id == -1){
        perror("Error shmget");
        exit(1);
    }

    shm_addr = (struct shm_segment *) shmat(segment_id, NULL, 0);

    printf("Shared Memory attached at address %p\n", shm_addr);

    shm_addr->first  = 0;
    shm_addr->last   = 1;
    shm_addr->count  = 0;

    socket_file_descriptor = socket(AF_INET, SOCK_STREAM, 0);
    if(socket_file_descriptor == -1){
        perror("socket");
        exit(1);
    }

    sv_socket_addr.sin_family = AF_INET;
    sv_socket_addr.sin_addr.s_addr = INADDR_ANY;
    sv_socket_addr.sin_port = htons(MYPORT);
    bzero(&(sv_socket_addr.sin_zero), sizeof(unsigned char) * 8);

    returned_int = bind(socket_file_descriptor, (struct sockaddr *) &(sv_socket_addr), sizeof(struct sockaddr) );
    if(returned_int == -1){
        perror("Error bind");
        exit(1);
    }    

    returned_int = listen(socket_file_descriptor, BACKLOG); //Modo pasivo al servidor

    while(1){
        socket_addr_size = sizeof(struct sockaddr_in);

        new_client_file_descriptor = accept(socket_file_descriptor, (struct sockaddr * ) &new_client_socket_addr, &socket_addr_size); //Aceptamos una conexión con el socket
        if(new_client_file_descriptor == -1){
            perror("Error accept");
            exit(1);
        }      

        printf("Server: I've got connection from: %s\n", inet_ntoa(new_client_socket_addr.sin_addr));

        process_id = fork();

        if(process_id != 0){
            while( strcmp(message_to_send, "disc") != 0 || strcmp(message_received, "disc") != 0 ){

                bytes_received = recv(new_client_file_descriptor, message_received, MAXDATASIZE, 0);
                if(bytes_received == -1){
                    perror("Error recv");
                    exit(1);
                }   
                message_received[bytes_received] = '\0';

                if(bytes_received == 0){
                    printf("Client Disconnected...\n");
                    close(new_client_file_descriptor);
                    exit(1);
                }

                printf("Message Received: %s\n", message_received);

                shm_message_reg(shm_addr,message_received, "client");

                printf("Type the message to sent: ");
                writeMessage(message_to_send);

                returned_int = send(new_client_file_descriptor, message_to_send, strlen(message_to_send) * sizeof(char), 0);
                if(returned_int == -1){
                    perror("Error send");
                    exit(1);
                }   

                printf("Message Sent!\n\n");

                shm_message_reg(shm_addr,message_to_send, "server");
            } 
        }

        else{
            close(new_client_file_descriptor);
        }

        while(waitpid(-1, NULL, WNOHANG) > 0);

    }
    return 0;
}