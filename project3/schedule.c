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
        err_msg("At: dispatch after first lock", err_flag);

        // forever loop to wait until there are processes to run
        while(p_waiting == 0)
        {
            pthread_cond_wait(&cmd_buf_not_empty, &cmd_queue_lock);
        }
        err_msg("At: dispatch after waiting for unlock loop", err_flag);

        // get currently running process
        current_process = running_processes[buff_prev];
        err_msg("At: dispatch after current process ran", err_flag);

        // signal and unlock queue
        pthread_cond_signal(&cmd_buf_not_full);
        pthread_mutex_unlock(&cmd_queue_lock);
        err_msg("At: dispatch after unlocks", err_flag);

        // run process
        execute_process(current_process);
        err_msg("At: dispatch after executed process", err_flag);

        // decrease waiting processes and move pointers
        p_waiting--; 
        buff_prev++;
        buff_prev = buff_prev % QUEUE_MAX_LEN;

        // clear current process after running
        current_process = NULL;
    }
}

// execute process
void execute_process(new_process p)
{
    // prepare path and program arguments
    char *path = p->program;
    char *cpu = "0"; 
    char data[200];
    int new_cpu = p->cpu_time_remaining;

    // convert to string
    sprintf(data, "%d", new_cpu);
    char *args[] = {path, cpu, NULL};
    err_msg("At: execute process and starting", err_flag);

    // make sure the command is legit
    if (!strcmp(p->program, "./dispatch"))
    {
        err_msg("At: execute process in strcomp", err_flag);
        args[1] = data;
        err_msg("At: execute process after strcmp", err_flag);
    }
    else
    {
        args[1] = "NULL";
    }
    err_msg("At: execute process about to do execv", err_flag);

    // execute program with execv
    pid_t child_process = fork();

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

    err_msg("At: execute process after execv", err_flag);

    // clear out cpu time remaining
    p->cpu_time_remaining = 0;

    err_msg("At: execute process about to update finished process", err_flag);
    // update the finished process queue
    update_finished_process(p);

    err_msg("At: execute process, finished", err_flag);
    // free up memory
    free(p);  

}

// update finished process list
void update_finished_process(new_process t)
{
    // create finished process
    new_process f_process = malloc(sizeof(n_process));

    // calculate cpu burst time
    f_process->finish_time = time(NULL);
    t->cpu_time = (int)(f_process->finish_time - t->cpu_first_time);

    // initialize finish process
    strcpy(f_process->program, t->program);
    f_process->arrival_time = t->arrival_time;
    f_process->cpu_time = t->cpu_time;
    f_process->priority = t->priority;
    f_process->cpu_first_time = t->cpu_first_time;
    f_process->turnaround_time = f_process->finish_time - f_process->arrival_time;
    f_process->cpu_time_remaining = 0;
    
    // calculate waiting time
    // make sure waiting time will not be a negative
    if (f_process->turnaround_time >= f_process->cpu_time)
    {
        f_process->waiting_time = f_process->turnaround_time - f_process->cpu_time;
    }
    else
    {
        f_process->waiting_time = 0;
    }

    // calculate response time
    f_process->response_time = f_process->cpu_first_time - f_process->cpu_time;

    // update finished process list
    finished_processes[finished_next] = f_process;
    finished_next++;
}

///////////////////////////////////////////////
// SCHEDULER
//////////////////////////////////////////////

// process one job and notify the dispatcher that there is a new job
void scheduler(int argc, char **argv)
{
    // lock process list
    pthread_mutex_lock(&cmd_queue_lock);
    err_msg("At: scheduler, first lock", err_flag);

    // wait until list is not full and still locked
    while(p_waiting == QUEUE_MAX_LEN)
    {
        pthread_cond_wait(&cmd_buf_not_full, &cmd_queue_lock);
    }
    err_msg("At: scheduler, after first lock loop", err_flag);

    // unlock 
    pthread_mutex_unlock(&cmd_queue_lock);
    err_msg("At: scheduler, after first unlock", err_flag);

    // initialize new process
    new_process new_p = init_process(argv);
    err_msg("At: scheduler after first init process", err_flag);

    // print info about jobs
    // HERE

    // add process to current list
    running_processes[buff_next] = new_p;
    err_msg("At: scheduler, after added process to list", err_flag);

    // lock to edit process list
    pthread_mutex_lock(&cmd_queue_lock);

    // inc counters
    p_waiting++;
    buff_next++;
    buff_next = buff_next % QUEUE_MAX_LEN;

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

    // remove newline?
    
    // init new process
    p->cpu_time = atoi(argv[2]);
    p->cpu_time_remaining = p->cpu_time;
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







