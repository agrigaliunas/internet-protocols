#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/stat.h>

#define MAXDATASIZE 100
#define MAXMESSAGES 1024

typedef char shm_message[MAXDATASIZE];

int segment_id = 0;
int flag = 1;

struct shm_segment{
    int first, last, count;
    shm_message messages[MAXMESSAGES];
};

void free_SHM(int signal_number){
    printf("\nFree SHM...\n");
    shmctl(segment_id,IPC_RMID,0);
    exit(1);
}

void writeMessage(char message[MAXDATASIZE]){
    fgets(message,MAXDATASIZE,stdin);
}


int main(){

    struct sigaction signal_action;
    struct shm_segment *shm_addr = NULL;
    key_t shm_key = ftok("server_shm.c", (int) 'A');

    // establecemos que pasa cuando CTRL+C
    memset(&(signal_action), 0, sizeof(struct sigaction));
    signal_action.sa_handler = &free_SHM;
    sigaction(SIGINT, &signal_action, NULL);

    segment_id = shmget(shm_key, (size_t) sizeof(struct shm_segment), IPC_CREAT | IPC_EXCL | S_IRUSR | S_IWUSR);
    if(segment_id == -1){
        perror("Error shmget");
        exit(1);
    }

    shm_addr = (struct shm_segment *) shmat(segment_id, NULL, 0);

    printf("Shared memory attached at address %p\n", shm_addr);

    shm_addr->first = 0;
    shm_addr->last = 0;
    shm_addr->count = 0;

    strcpy(shm_addr->messages[0],"Hola!\n");
    shm_addr->count++;

    while(1){
        if(shm_addr->count < MAXMESSAGES){
            shm_addr->first = (shm_addr->first + 1)%MAXMESSAGES;
            printf("Escribe mensaje numero %d:",shm_addr->first);
            writeMessage(shm_addr->messages[shm_addr->first]);
            shm_addr->count++;
        }
    
    }
    free_SHM(0);

    return 0;
}
