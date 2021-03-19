// header file

// libraries
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include <limits.h>
#include <sys/types.h>
#include <stdbool.h>

// global defines
#define PROGRAM_LEN_MAX 500 // max program length
#define QUEUE_MAX_LEN 12    // max waiting queue length

// global process list pointers
int buff_next;          // next free spot in running_processes
int buff_curr;          // current process spot in running_processes
int buff_prev;          // next process to run
int p_waiting;          // number of processes waiting to run
int finished_next;      // next free spot in the finished processes list
bool err_flag;  // if error messages will be printed

// global locks
pthread_mutex_t cmd_queue_lock;  // Lock for critical sections 
pthread_cond_t cmd_buf_not_full; // Condition variable for buf_not_full 
pthread_cond_t cmd_buf_not_empty; // Condition variable for buf_not_empty 

// scheduling policies
enum s_policy
{
    fcfs,
    sjf,
    priority,
} policy;


// struct for processes
typedef struct
{
    int cpu_time;                   // cpu time
    int cpu_time_remaining;         // remaining cpu time
    int cpu_first_time;             // first time on cpu
    int priority;                   // priority 
    int response_time;              // response time
    int waiting_time;               // waiting time
    int turnaround_time;            // turnaround time
    int finish_time;                // finishing time
    time_t arrival_time;            // arrival time
    char program[PROGRAM_LEN_MAX];  // program name
} n_process;

// global queue typedefs
typedef n_process *new_process;
typedef n_process *finished_process;

// process queues
new_process running_processes[QUEUE_MAX_LEN];
new_process finished_processes[10000];
new_process current_process;

// dispatcher functions
void execute_process(new_process p);
void update_finished_process(new_process t);
void *dispatch(void *point);

// scheduler functions
void scheduler(int argc, char **argv);
new_process init_process(char **argv);
void sort_process_list(new_process *proc_list);

// policy functions

// reporting stats functions

// extra programs
void err_msg(char *item, bool err_flag);

// 