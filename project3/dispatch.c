
/* 
 * COMP7500/7506
 * Project 3: 
 * ./dispatch 7
 * This will sleep for 7 seconds. Capability for it to take 
 * multiple processes at the same time is setup but not necessary.
 * It should only take one process at a time.
 */
#include <stdio.h> 
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

void main(int argc, char *argv[])
{
  // use cpu for i amount of time
  //printf("your number is: %s", argv[1]);
  sleep(3);
}
