#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/wait.h>

#define MYPORT 3490  // Puerto al cual nos conectaremos

#define BACKLOG 5 // Cantidad de conecciones en la cola.
#define MAXDATASIZE 100

int main(){
    int socket_file_descriptor;
    int new_file_descriptor; // escuchamos en sockfd, nueva coneccion en new_fd
    struct sockaddr_in my_addr; 
    struct sockaddr_in their_addr;
    int sin_size;
    int numbytes;
    char buf[MAXDATASIZE];

    if ((socket_file_descriptor = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1){
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


    while(1){
        struct sockaddr_in host_address;
        int host_address_size;
        unsigned char *address_holder;
        char message[]="Mensaje enviado";
        char buffer[256];

        memset((void *)&host_address, 0, sizeof(host_address));
        host_address.sin_family=AF_INET;
        host_address.sin_port=htons(MYPORT);
        address_holder=(unsigned char*)&host_address.sin_addr.s_addr;
        address_holder[0]=127;
        address_holder[1]=0;
        address_holder[2]=0;
        address_holder[3]=1;

        memset((void*)&host_address, 0, sizeof(host_address));
        host_address.sin_family=AF_INET;
        host_address_size=sizeof(host_address);

        if(recvfrom(socket_file_descriptor, buffer, 255, 0, (struct sockaddr*)&host_address, &host_address_size)<0) // Lee e imprime de donde obtuvo los datos.
        {
            printf("%d", errno);
            perror("recvfrom()");
            return 1;
        }

        printf("Mensaje proveniente del port %d: %s, direccion %d.%d.%d.%d.\n", MYPORT, buffer, address_holder[0], address_holder[1], address_holder[2], address_holder[3]);
        
    }
    return 0;
}
