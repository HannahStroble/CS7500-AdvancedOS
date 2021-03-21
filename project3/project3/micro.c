///////////////////////////////////////////////////////////////////
// COMP7500 - Project 3 - A Pthread-based Batch Scheduling System
// Name: Hannah Reinbolt
// Date: 3/14/2021
// Description: Microbatch program that will run for X user defined time.
// Note: Referenced code provided by Dr. Qin
//
// How to run:
// >> gcc -o micro micro.c -lpthread
// >> ./micro 3
// This will run for 3 seconds.
//
///////////////////////////////////////////////////////////////////


// libraries
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdbool.h>
#include <sys/wait.h>


////////////////////////////////////////////////
// name: main
// use: runs some process for user defined number of seconds
// description: run calculations on cpu as a child process counts time for user defined number of seconds
//
// input: int argc
// input description: number of argv
// input: char *argv[]
// input description: char array of command line input
// output: int
// output description: dummy value
////////////////////////////////////////////////
void main(int argc, char *argv[])
{
  // create fork
    pid_t child, pid;
    int count = 0;
    child = fork();

    // start child fork
    if (child < 0)
    {
        printf("Failed to create a new process.\n");
    }
    // execute child fork
    // as child is sleeping, the cpu will be working on the parent in parallel
    // this will be consistently working the cpu for a user defined amount of time
    else if (child == 0)
    {
        // sleep for user defined amount of time
        sleep(atoi(argv[1]));
        exit(0);
    }
    // wait for child process to complete before updating finished list
    while ((pid = wait(&count)) > 0)
    {
      // do this cpu calculation for as long as waiting for child
      double calculation = 1356/123098;
    }

}
