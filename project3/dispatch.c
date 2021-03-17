
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

int main(int argc, char *argv[] )
{
  int i = 1;
  printf("A process is running ...\n");
  argc--;

  /* Print all the arguments passed from the commandline */
  while (argc > 0) {
    printf ("Argument %d is: %s\n", i, argv[i]);
    sleep(atoi(argv[i]));
    i++;
    argc--;
  }

  return 0;
}
