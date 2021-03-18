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

// local libraries
#include "commandline.h"
#include "schedule.h"

//////////////////////////////////////////////////
// MOVED VARS
/////////////////////////////////////////////////

typedef unsigned int u_int; 

//#define LOW_ARRIVAL_RATE /* Long arrivel-time interval */
#define LOW_SERVICE_RATE   /* Long service time */

/* 
 * Static commands are submitted to the job queue.
 * When comment out the following macro, job are submitted by users.
 */
//#define STATIC_COMMAND 

#define CMD_BUF_SIZE 10 /* The size of the command queueu */
#define NUM_OF_CMD   5  /* The number of submitted jobs   */
#define MAX_CMD_LEN  512 /* The longest commandline length */

/* 
 * When a job is submitted, the job must be compiled before it
 * is running by the executor thread (see also executor()).
 */
//void *commandline( void *ptr ); /* To simulate job submissions and scheduling */
//void *executor( void *ptr );    /* To simulate job execution */

pthread_mutex_t cmd_queue_lock;  /* Lock for critical sections */
pthread_cond_t cmd_buf_not_full; /* Condition variable for buf_not_full */
pthread_cond_t cmd_buf_not_empty; /* Condition variable for buf_not_empty */

/* Global shared variables */
u_int buf_head;
u_int buf_tail;
u_int count;
char *cmd_buffer[CMD_BUF_SIZE];

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
    count = 0; 
    buf_head = 0;  
    buf_tail = 0; 

    /* Create two independent threads:command and executors */
    iret1 = pthread_create(&command_thread, NULL, commandline, (void*) message1);
    //iret2 = pthread_create(&executor_thread, NULL, executor, (void*) message2);

    /* Initialize the lock the two condition variables */
    pthread_mutex_init(&cmd_queue_lock, NULL);
    pthread_cond_init(&cmd_buf_not_full, NULL);
    pthread_cond_init(&cmd_buf_not_empty, NULL);
     
    /* Wait till threads are complete before main continues. Unless we  */
    /* wait we run the risk of executing an exit which will terminate   */
    /* the process and all threads before the threads have completed.   */
    pthread_join(command_thread, NULL);
    //pthread_join(executor_thread, NULL); 

    printf("command_thread returns: %d\n",iret1);
    //printf("executor_thread returns: %d\n",iret2);
    exit(0);

    ///////////////////////////////////////////////////
    ///////////////////////////////////////////////////


  // return
  return 0;

}


