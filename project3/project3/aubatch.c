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


// libraries
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/types.h>
#include <stdbool.h>

// local libraries
#include "commandline.h"
#include "schedule.h"


//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
///////////    MAIN FUNCTION
//////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////
// name: main
// use: creates threads and calls main program
// description: creates threads and calls main program to work together
//
// input: int argc
// input description: number of argv
// input: char *argv[]
// input description: char array of command line input
// output: int
// output description: dummy value
////////////////////////////////////////////////
int main(int argc, char *argv[])
{ 

    // make sure there are no parameters
  if (argc > 1)
  {
    printf("Too many input parameters! This program only allows 0. Try again.");
    exit(0);
  }

    //print welcome message
	  printf("Welcome to Hannah's batch job scheduler Version 1.0\n");
	  printf("Type 'help' to find more about AUbatch commands.\n");

    // create threads
    pthread_t command_thread, executor_thread; /* Two concurrent threads */

    // create messages and returns
    char *message1 = "Command Thread";
    char *message2 = "Executor Thread";
    int  iret1, iret2;

    // initialize global variables
    p_waiting = 0; 
    buff_next = 0;  
    buff_prev = 0;
    err_flag = 0; 
    b_job = 0;
    policy = fcfs;

    // Create two independent threads:command and executors
    iret1 = pthread_create(&command_thread, NULL, commandline, (void*) message1);
    iret2 = pthread_create(&executor_thread, NULL, dispatch, (void*) message2);

    // Initialize the lock the two condition variables 
    pthread_mutex_init(&cmd_queue_lock, NULL);
    pthread_cond_init(&cmd_buf_not_full, NULL);
    pthread_cond_init(&cmd_buf_not_empty, NULL);
     
    // Wait till threads are complete before main continues. Unless we 
    // wait we run the risk of executing an exit which will terminate
    // the process and all threads before the threads have completed.
    pthread_join(command_thread, NULL);
    pthread_join(executor_thread, NULL); 

    // print return of threads
    printf("command_thread returns: %d\n",iret1);
    printf("executor_thread returns: %d\n",iret2);
    exit(0);

  // exit
  return 0;
}


