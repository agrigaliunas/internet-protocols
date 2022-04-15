#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/stat.h>

#define MAXDATASIZE 100
#define MAXMESSAGES 1024

typedef char shm_message[MAXDATASIZE];

struct shm_segment{
    int first, last, count;
    shm_message messages[MAXMESSAGES];
};

void writeMessage(char message[MAXDATASIZE]){
    fgets(message,MAXDATASIZE,stdin);
}


int main(){
    int segment_id = 0;
    struct shm_segment *shm_addr = NULL;

    key_t shm_key = ftok("server_shm.c", (int) 'A'); // creo la clave compartida

    segment_id = shmget(shm_key, (size_t) sizeof(struct shm_segment), IPC_EXCL | S_IRUSR | S_IWUSR);

    // shmget crea la zona de memoria
    
    // IPC_EXCL si la memoria compartida no existe, se establece. Si no, da erorr
    // S_IRUSR y S_IWUSR son flags para permisos de lectura
    
    if(segment_id == -1){
        perror("Error shmget");
        exit(1);
    }

    shm_addr = (struct shm_segment *) shmat(segment_id, NULL, 0);
    // se castea a *shm_segment ya que shmat retorna un tipo char*
    // shmat nos da el puntero que apunta a la zona de memoria.
    // asocia la mem compartida a la zona de datos del programa

    printf("Shared memory attached at address %p\n", shm_addr);

    while(1){
        if(shm_addr->count > 0){
            printf("Nuevo mensaje: %s\n", shm_addr->messages[shm_addr->last]);
            shm_addr->last = (shm_addr->last + 1) % MAXMESSAGES;
            shm_addr->count--; 
        }

        else sleep(5);
    }

    return 0;
}