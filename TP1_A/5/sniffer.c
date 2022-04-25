#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/stat.h>

#define MAXDATASIZE 100
#define MAXMESSAGES 1024
#define MAXDATASIZE 100
#define MAXMESSAGES 1024
#define MAXALIASSIZE 15

int segment_id = 0;

typedef char shm_message[MAXDATASIZE];

struct shm_register{
    char alias[MAXALIASSIZE];
    shm_message message;
};

struct shm_segment{
    int first, last, count;
    struct shm_register registers[MAXMESSAGES];
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
    struct shm_segment * shm_addr = NULL;
    key_t shmKey = ftok("sniffer.c", (int) 'A');

    char alias[MAXALIASSIZE];
    char message[MAXDATASIZE];

    memset( &(signal_action), 0, sizeof(struct sigaction) );
    signal_action.sa_handler = &free_SHM;
    sigaction(SIGINT, &signal_action, NULL);

    segment_id = shmget( shmKey, (size_t) sizeof(struct shm_segment), IPC_CREAT | IPC_EXCL | S_IRUSR | S_IWUSR );
    if(segment_id == -1){
        perror("Error shmget");
        exit(1);
    }

    shm_addr = (struct shm_segment *) shmat(segment_id, NULL, 0);

    printf("Shared Memory attached at address %p\n", shm_addr);

    while(1){
        if(shm_addr->count > 0){
            strcpy(alias,(shm_addr->registers[shm_addr->last]).alias);
            strcpy( message,(shm_addr->registers[shm_addr->last]).message);

            printf("%s:%s\n", alias, message);
            shm_addr->last = (shm_addr->last + 1) % MAXMESSAGES;
            shm_addr->count--;
        }
        else sleep(5);
    }

    free_SHM(0);

    return 0;
}