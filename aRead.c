#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <sys/errno.h>
#include <assert.h>
#include "queue.h"
#include "disk.h"
#include "uthread.h"

queue_t      pending_read_queue;
volatile unsigned int sum = 0;
int numSummed = 0;
//array of integers
// You may add your own variables here

void interrupt_service_routine () {
  void* val;
  void (*callback)(void*,void*);
  queue_dequeue (pending_read_queue, &val, NULL, &callback);
  callback (val, NULL);
}

void handle_read (void* resultv, void* not_used) {
    int *result =(int*) resultv;

    
    sum+= *result;
    
    numSummed++;
}

int main (int argc, char** argv) {

  // Command Line Arguments
  static char* usage = "usage: aRead num_blocks";
  int num_blocks;
  char *endptr;
  if (argc == 2)
    num_blocks = strtol (argv [1], &endptr, 10);
  if (argc != 2 || *endptr != 0) {
    printf ("argument error - %s \n", usage);
    return EXIT_FAILURE;
  }

  // Initialize
  uthread_init (1);
  disk_start (interrupt_service_routine);
  pending_read_queue = queue_create();

  // Sum Blocks
  /* TODO: Read disk blocks */
    int *intArray = malloc(sizeof(int)* num_blocks);
    for(int blockno = 0; blockno <num_blocks; blockno++){
        
        
        queue_enqueue(pending_read_queue, &intArray[blockno], NULL, handle_read);
    disk_schedule_read(&intArray[blockno], blockno);
    }
    while (numSummed != num_blocks){
        
        
    }
    free(intArray);
    
  printf("%d\n", sum);
  return 0;
}
