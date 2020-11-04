#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/errno.h>
#include <assert.h>
#include "uthread.h"
#include "queue.h"
#include "disk.h"

queue_t pending_read_queue;
int iterator;
volatile int is_read_pending;

void interrupt_service_routine() {
    void* val;
    void* count;
    void (*callback)(void*,void*);
    queue_dequeue (pending_read_queue, &val, &count, &callback);
    callback (val, count);
}

void handleOtherReads(void *resultv, void *countv) {
    int *currentCount = (int*) countv;
    int *result =(int*) resultv;
    *currentCount = *currentCount -1;
    if(*currentCount == 0){
        printf("%d",*result);
        exit(EXIT_SUCCESS);
    } else {
        queue_enqueue(pending_read_queue, resultv, countv, handleOtherReads);
        disk_schedule_read(resultv, *result);
        
    }
    
    
    
    
}

void handleFirstRead(void *resultv, void *countv) {
    int *currentCount = (int*) countv;
    int *result =(int*) resultv;
    *currentCount = *result;
    queue_enqueue(pending_read_queue, resultv, countv, handleOtherReads);
    disk_schedule_read(resultv, *result);
    
    
    
  
}

int main(int argc, char **argv) {
  // Command Line Arguments
  static char* usage = "usage: treasureHunt starting_block_number";
  int starting_block_number;
  char *endptr;
  if (argc == 2)
    starting_block_number = strtol (argv [1], &endptr, 10);
  if (argc != 2 || *endptr != 0) {
    printf ("argument error - %s \n", usage);
    return EXIT_FAILURE;
  }

  // Initialize
  uthread_init (1);
  disk_start (interrupt_service_routine);
  pending_read_queue = queue_create();
    
    int nextAddress;
    int currentCount;
    queue_enqueue(pending_read_queue, &nextAddress, &currentCount, handleFirstRead);
    disk_schedule_read(&nextAddress, starting_block_number);
    
    


  // Start the Hunt
  // TODO
  while (1); // infinite loop so that main doesn't return before hunt completes
}
