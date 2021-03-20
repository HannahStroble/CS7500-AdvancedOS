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

    // set cpu time to time variable
    if (p->cpu_first_time == 0)
    {
        p->cpu_first_time = time(NULL);
    }

    // wait for child process to complete before updating finished list
    while ((wpid = wait(&child_status)) > 0);

    // create finished process
    memcpy(&finished_processes[finished_next], &running_processes[buff_prev], sizeof(current_process));

    // update finished process
    finished_processes[finished_next]->finish_time = time(NULL);
    finished_processes[finished_next]->turnaround_time = finished_processes[finished_next]->finish_time - finished_processes[finished_next]->arrival_time;
    if (finished_processes[finished_next]->turnaround_time)
    {
        finished_processes[finished_next]->waiting_time = finished_processes[finished_next]->turnaround_time - finished_processes[finished_next]->cpu_time;
    }
    else
    {
        finished_processes[finished_next]->waiting_time = 0;
    }
    finished_processes[finished_next]->response_time = finished_processes[finished_next]->cpu_first_time - finished_processes[finished_next]->arrival_time;

    // update finished process head
    finished_next++;

    // update running process to finished cpu time
    running_processes[buff_prev]->cpu_time_remaining = 0;

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
    char *n_policy = get_policy();
    printf("Job %s was submitted.\n", new_p->job_name);
    printf("Total number of jobs in the queue: %d\n", p_waiting+1);
    printf("Expected waiting time: %d seconds\n", get_wait_time());
    printf("Scheduling Policy: %s.\n", n_policy);

    // add process to current list
    running_processes[buff_next] = new_p;

    // lock to edit process list
    pthread_mutex_lock(&cmd_queue_lock);

    // inc counters
    p_waiting++;
    buff_next++;

    sort_process_list(running_processes);
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

    // create job name
    char new_name[200];
    int id = buff_next;

    // convert to string
    sprintf(new_name, "job%d", id);
    
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
    strcpy(p->job_name, new_name);
    strcpy(p->program, argv[1]);

    // return process
    return p;
}

//////////////////////////////////////////////////////////////////////
////////// POLICY
//////////////////////////////////////////////////////////////////////


// sort processes by scheduling policy
void sort_process_list(new_process *proc_list)
{
    // sort by index
    int i;
    void *sort_algo = switch_to_policy;

    if (!b_job)
    {
        i = buff_prev + 1;
    }
    else
    {
        i = buff_prev;
    }

    // quick sort
    qsort(&proc_list[i], buff_next-i, sizeof(new_process), sort_algo);
    //print_jobs(running_processes);
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
    else
    {
        return (p_a->arrival_time - p_b->arrival_time);
    }

    // didn't hit a valid policy
    printf("Unknown scheduling policy. Will not sort. Cancelled.\n");
    return 0;
}

////////////////////////////////////////////////////////
////// METRICS
////////////////////////////////////////////////////////

// calculate estimated wait time
int get_wait_time()
{
    // variables
    int wait = 0;
    int i;

    // loop through current processes
    for (i = buff_prev; i < buff_next; i++)
    {
        wait += running_processes[i]->cpu_time_remaining;
    }

    // return wait time
    return wait;
}

// report statistics 
void performance_metrics()
{
    // check if there are any finished processes
    if (!finished_next)
    {
        printf("No jobs were finished. Cannot calculate performance metrics.\n");
        return;
    }

    // performance variables
    int total_wait_time = 0;
    int total_turn_time = 0;
    int total_response_time = 0;
    int total_cpu_time = 0;

    // create process
    new_process f_process;
    int index;

    // iterate and update stats on all finished processes
    for (index = 0; index < finished_next; index++)
    {
        // look at first process
        f_process = finished_processes[index];

        // update totals
        total_wait_time += f_process->waiting_time;
        total_turn_time += f_process->turnaround_time;
        total_response_time += f_process->response_time;
        total_cpu_time += f_process->cpu_time;
    }

    // print average metrics
    printf("Total number of jobs submitted: %d\n", finished_next + (buff_next - buff_prev));
    printf("Average turnaround time: %.3f seconds\n", total_turn_time / (float)index);
    printf("Average CPU time: %.3f seconds\n", total_cpu_time / (float)index);
    printf("Average waiting time: %.3f seconds\n", total_wait_time / (float)index);
    printf("Average response time: %.3f seconds\n", total_response_time / (float)index);
    printf("Throughput: %.3f No./second\n", 1/(total_turn_time / (float)index));

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

char *get_policy()
{
    if (policy == fcfs)
    {
        return "FCFS";
    }
    else if (policy == sjf)
    {
        return "SJF";
    }
    else if (policy == priority)
    {
        return "PRIORITY";
    }
    return "NULL";
}








