#include <stdio.h>
#include <unistd.h>
#include <stdlib.h> 
#include <sys/shm.h>
#include <sys/stat.h>

typedef char shm_message[200]; 

struct shm_segment{
   int first, last, count;
   shm_message messages[1024];
};


void consumer(){
  int segment_id;
  struct shm_segment *shared_memory;
  struct shmid_ds shmbuffer;
  int segment_size;
  const int shared_segment_size = 0x3200C;
  key_t lv_key;

  lv_key = ftok("producer.cpp",'B');

  /* Allocate a shared memory segment.  */
  segment_id = shmget (lv_key, shared_segment_size,
		       IPC_EXCL | S_IRUSR | S_IWUSR);

  /* Attach the shared memory segment.  */
  shared_memory = (struct shm_segment*) shmat (segment_id, 0, 0);
  printf ("shared memory attached at address %p\n", shared_memory);

  while (1){
    if ( shared_memory->count > 0 ){
      int lv_aux = shared_memory->last;
      
      printf("Leido de shm: %s\n",shared_memory->messages[lv_aux]);
      shared_memory->last = ( lv_aux + 1 ) % 200;
      shared_memory->count--;
    }
    else sleep(10);
  }
  
} 


int main(){ 
  int fds;
  pid_t pid;

  consumer ();
  return 0;
} 

