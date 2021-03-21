
/* 
 * COMP7500/7506
 * Project 3: 
 * ./dispatch 7
 * This will sleep for 7 seconds. Capability for it to take 
 * multiple processes at the same time is setup but not necessary.
 * It should only take one process at a time.
 */
///////////////////////////////////////////////////////////////////
// COMP7500 - Project 3 - A Pthread-based Batch Scheduling System
// Name: Hannah Reinbolt
// Date: 3/14/2021
// Description: Run everything in main, sync main variables, execute threads.
// Note: Referenced code provided by Dr. Qin
//
// How to run:
// >> make
// >> ./aubatch
//
///////////////////////////////////////////////////////////////////
#include <stdio.h> 
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <math.h>

void main(int argc, char *argv[])
{
  // use cpu for i amount of time
  //printf("your number is: %s", argv[1]);
  int i;
  while (i < atoi(argv[1]))
  {
    int j;
    for (j=0; j<214000000; j++)
    {
      volatile double x = 9999999.0;
      volatile double y = cbrt(x);
      j++;
    }
    i++;
  }
  sleep(atoi(argv[1]));
}
