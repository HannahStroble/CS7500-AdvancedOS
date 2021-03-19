/////////////////////////////////////////////////////////////////////
// COMP7500 - Project 3 - A Pthread-based Batch Scheduling System
// Name: Hannah Reinbolt
// Date: 3/14/2021
// Description: Contains scheduling and dispatching modules.
////////////////////////////////////////////////////////////////////

// libraries
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdbool.h>
#include <sys/wait.h>

// local libraries
#include "schedule.h"

///////////////////////////////////////////////////////////////
// DISPATCHER
///////////////////////////////////////////////////////////////

// Executes jobs on the running_processes queue and adds to finished_processes when finished.
void *dispatch(void *point)
{
    while(1)
    {
        // lock shared queue so scheduler can't access
        pthread_mutex_lock(&cmd_queue_lock);

        // forever loop to wait until there are processes to run
        while(p_waiting == 0)
        {
            pthread_cond_wait(&cmd_buf_not_empty, &cmd_queue_lock);
        }

        // get currently running process
        current_process = running_processes[buff_prev];

        // signal and unlock queue
        pthread_cond_signal(&cmd_buf_not_full);
        pthread_mutex_unlock(&cmd_queue_lock);

        // run process
        execute_process(current_process);

        // decrease waiting processes and move pointers
        p_waiting--; 
        buff_prev++;
        buff_prev %= QUEUE_MAX_LEN;

        // clear current process after running
        current_process = NULL;
    }
}

// execute process
void execute_process(new_process p)
{
    // prepare path and program arguments
    char *path = p->program;
    char data[200];
    int new_cpu = p->cpu_time_remaining;

    // convert to string
    sprintf(data, "%d", new_cpu);
    char *args[] = {path, "0", NULL};

    // make sure the command is legit
    if (!strcmp(p->program, "./dispatch"))
    {
        args[1] = data;
    }

    // execute program with execv
    pid_t child_process, wpid;
    int child_status = 0;
    child_process = fork();

    // start child fork
    if (child_process < 0)
    {
        printf("Failed to create a new process.\n");
    }
    else if (child_process == 0)
    {
        execv(path, args);
        printf("EXECV FAILED! EXITING...");
        exit(0);
    }

    if (p->cpu_first_time == 0)
    {
        p->cpu_first_time = time(NULL);
    }

    // wait for child process to complete before updating finished list
    while ((wpid = wait(&child_status)) > 0);

    // clear out cpu time remaining
    //p->cpu_time_remaining = 0;

    // create finished process
    memcpy(&finished_processes[finished_next], &running_processes[buff_prev], sizeof(current_process));

    // update finished process list
    finished_next++;

    // free up memory
    //free(p);  
}

///////////////////////////////////////////////
// SCHEDULER
//////////////////////////////////////////////

// process one job and notify the dispatcher that there is a new job
void scheduler(int argc, char **argv)
{
    // lock process list
    pthread_mutex_lock(&cmd_queue_lock);

    // wait until list is not full and still locked
    while(p_waiting == QUEUE_MAX_LEN)
    {
        pthread_cond_wait(&cmd_buf_not_full, &cmd_queue_lock);
    }

    // unlock 
    pthread_mutex_unlock(&cmd_queue_lock);

    // initialize new process
    new_process new_p = init_process(argv);

    // print info about jobs
    // HERE

    // add process to current list
    running_processes[buff_next] = new_p;

    // lock to edit process list
    pthread_mutex_lock(&cmd_queue_lock);

    // inc counters
    p_waiting++;
    buff_next++;

    //sort_process_list(running_processes);
    buff_next %= QUEUE_MAX_LEN;

    // sort process list
    //sort_process_list(running_processes);

    // unlock shared process list
    pthread_cond_signal(&cmd_buf_not_empty);
    pthread_mutex_unlock(&cmd_queue_lock);
}


// initialize new process for scheduler
new_process init_process(char **argv)
{
    // init size of new process
    new_process p = malloc(sizeof(n_process));
    
    // init new process
    p->cpu_time = atoi(argv[2]);
    p->cpu_time_remaining = atoi(argv[2]);
    p->cpu_first_time = 0;
    p->priority = atoi(argv[3]);
    p->response_time = 0;
    p->waiting_time = 0;
    p->turnaround_time = 0;
    p->finish_time = 0;
    p->arrival_time = time(NULL);
    strcpy(p->program, argv[1]);

    // return process
    return p;
}

// sort processes by scheduling policy
void sort_process_list(new_process *proc_list)
{
    // sort by index
    int i;

    if(!b_job)
    {
        i = buff_prev + 1;
    }
    else
    {
        i = buff_prev;
    }

    // quick sort
    qsort(&running_processes[i], buff_next-i, sizeof(new_process), switch_to_policy);
}

// sorting
int switch_to_policy(const void *a, const void *b)
{
    new_process p_a = *(new_process *)a;
    new_process p_b = *(new_process *)b;

    if (policy == fcfs)
    {
        return (p_a->arrival_time - p_b->arrival_time);
    }
    else if (policy == sjf)
    {
        return (p_a->cpu_time_remaining - p_b->cpu_time_remaining);
    }
    else if (policy == priority)
    {
        return (p_b->priority - p_a->priority);
    }

    // didn't hit a valid policy
    printf("Unknown scheduling policy. Will not sort. Cancelled.\n");
    return 0;
}


////////////////////////////////////////////////////////
//// EXTRA 
///////////////////////////////////////////////////////

// error printer
void err_msg(char *item, bool err_flag)
{
    // if flag true then print message
    if (err_flag == true)
    {
        printf("%s", item);
        printf("\n");
    }
}








