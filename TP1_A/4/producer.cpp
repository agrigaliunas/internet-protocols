#include <stdio.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/shm.h>
#include <signal.h>
#include <string.h>

typedef char shm_message[200]; 

struct shm_segment{
   int first, last, count;
   shm_message messages[1024];
};

int segment_id;
struct shm_segment *shared_memory;

int flag = 1;
void Free_SHM (int signal_number){
//  shmdt (shared_memory);

  /* Deallocate the shared memory segment.  */
  shmctl (segment_id, IPC_RMID, 0);
  flag = 0;
}

void producer(){ 
// Registrar funcion que limpie SHM segment si Ctrl+C
  struct sigaction sa;
  memset (&sa, 0, sizeof (sa));
  sa.sa_handler = &Free_SHM;
  sigaction (SIGINT, &sa, NULL);

  struct shmid_ds shmbuffer;
  int segment_size;
  const int shared_segment_size = 0x3200C;
  key_t lv_key;

  lv_key = ftok("producer.cpp",'B');

  /* Allocate a shared memory segment.  */
  segment_id = shmget (lv_key, shared_segment_size,
		       IPC_CREAT | IPC_EXCL | S_IRUSR | S_IWUSR);

  /* Attach the shared memory segment.  */
  shared_memory = (struct shm_segment*) shmat (segment_id, 0, 0);
  printf ("shared memory attached at address %p\n", shared_memory);

  shared_memory->first = 0;
  shared_memory->count = 0;
  shared_memory->last = 1;

  while(flag){
      if ( shared_memory->count < 200 ){
        shared_memory->first = ( shared_memory->first + 1 ) % 200;
        int lv_aux = shared_memory->first;
        fgets(shared_memory->messages[lv_aux], 200, stdin);
        shared_memory->count++;
      }    
//  sprintf (shared_memory->messages[0], "Hello, world.");
    }
} 


int main(){ 
  int fds;
  pid_t pid;

  producer();

  return 0;
} 

