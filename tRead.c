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
unsigned int sum = 0;

void interrupt_service_routine () {
    void *temp;
    queue_dequeue (pending_read_queue,&temp, NULL, NULL);
    uthread_unblock(temp);
}

void *read_block (void *arg) {
    int* blockNo = arg;
    int readVal;
    
    disk_schedule_read(&readVal, *blockNo);
    queue_enqueue(pending_read_queue,uthread_self(),NULL,NULL);
    uthread_block();
    
    sum += readVal;

  return NULL;
}

int main (int argc, char** argv) {

  // Command Line Arguments
    static char* usage = "usage: tRead num_blocks";
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
    uthread_t threads[num_blocks];
    int blockNums[num_blocks];
    for (int num = 0; num < num_blocks; num++) {
        blockNums[num] = num;
        threads[num] = uthread_create(read_block, &blockNums[num]);
        
    }
    for (int num = 0; num < num_blocks; num++) {
        uthread_join(threads[num],NULL);
    }
    printf("%d", sum);
}

