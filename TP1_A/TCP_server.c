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

main()
{
 int sockfd, new_fd; // escuchamos en sockfd, nueva coneccion en new_fd
 struct sockaddr_in my_addr; 
 struct sockaddr_in their_addr;
 socklen_t sin_size;
 int numbytes;
 char buf[MAXDATASIZE];


 if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
 {
  perror("socket");
  exit(1);
 }

 my_addr.sin_family = AF_INET;
 my_addr.sin_port   = htons(MYPORT);
 my_addr.sin_addr.s_addr = INADDR_ANY;
 bzero(&(my_addr.sin_zero), 8);

 if (bind(sockfd, (struct sockaddr *)&my_addr, sizeof(struct sockaddr)) == -1)
 {
  perror("bind");
  exit(1);
 }

 if (listen(sockfd, BACKLOG) == - 1)
 {
  perror("listen");
  exit(1);
 }

 while(1)
 {
  sin_size = sizeof(struct sockaddr_in);
  if ((new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size)) == -1)
  {
   perror("accept");
   continue;
  }
  printf("server: got connection from %s\n", inet_ntoa(their_addr.sin_addr));

  if(!fork())
  {
   while(1)
   {
   if (send(new_fd, "Hello, world!\n", 14, 0) == -1) perror("send");
   else printf("Enviado\n");
   if ((numbytes=recv(new_fd, buf, MAXDATASIZE, 0)) == -1)
   {
    perror("recv");
    exit(1);
   }

   buf[numbytes] = '\0';

   printf("Received: %s\n", buf);
  }
  }  
  else close(new_fd);

  while(waitpid(-1, NULL, WNOHANG) > 0);
 }
}
