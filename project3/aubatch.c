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
#include <sys/types.h>
#include <stdbool.h>

// local libraries
#include "commandline.h"
#include "schedule.h"

//////////////////////////////////////////////////
// MOVED VARS
/////////////////////////////////////////////////

/* 
 * When a job is submitted, the job must be compiled before it
 * is running by the executor thread (see also executor()).
 */

///////////////////////////////////////////////////////

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

  ////////////////////////////////////////////////////
  ////////////////////////////////////////////////////

    pthread_t command_thread, executor_thread; /* Two concurrent threads */
    char *message1 = "Command Thread";
    char *message2 = "Executor Thread";
    int  iret1, iret2;

    /* Initilize count, two buffer pionters */
    p_waiting = 0; 
    buff_next = 0;  
    buff_prev = 0;
    err_flag = 1; 

    /* Create two independent threads:command and executors */
    iret1 = pthread_create(&command_thread, NULL, commandline, (void*) message1);
    iret2 = pthread_create(&executor_thread, NULL, dispatch, (void*) message2);

    /* Initialize the lock the two condition variables */
    pthread_mutex_init(&cmd_queue_lock, NULL);
    pthread_cond_init(&cmd_buf_not_full, NULL);
    pthread_cond_init(&cmd_buf_not_empty, NULL);
     
    /* Wait till threads are complete before main continues. Unless we  */
    /* wait we run the risk of executing an exit which will terminate   */
    /* the process and all threads before the threads have completed.   */
    pthread_join(command_thread, NULL);
    pthread_join(executor_thread, NULL); 

    printf("command_thread returns: %d\n",iret1);
    printf("executor_thread returns: %d\n",iret2);
    exit(0);

    ///////////////////////////////////////////////////
    ///////////////////////////////////////////////////


  // return
  return 0;

}


