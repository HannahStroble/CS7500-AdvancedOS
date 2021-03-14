///////////////////////////////////////////////////////////////////
// COMP7500 - Project 3 - A Pthread-based Batch Scheduling System
// Name: Hannah Reinbolt
// Date: 3/14/2021
// Description: Run everything in main, sync main variables.
///////////////////////////////////////////////////////////////////

// libraries
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>

// local libraries
#include "commandline.h"
#include "schedule.h"

int main(int argc, char *argv[])
{ 
  // variables
  char usr_input[256];

  // make sure there are no parameters
  if (argc > 1)
  {
    printf("Too many input parameters! This program only allows 0. Try again.");
    exit(1);
  }

  // start forever loop
  while(1)
  {
    // get user input 
    fgets(usr_input, sizeof(usr_input), stdin);
    printf("usr input: %s", usr_input);

    // check for help menu
    exit(1);
  }


  // return
  return 0;

}


