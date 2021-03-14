//////////////////////////////////////
// COMP7500 - Project 3 - A Pthread-based Batch Scheduling System
// Name: Hannah Reinbolt
// Date: 3/14/2021
// Description: Interact with the users
//////////////////////////////////////////


// libraries
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>

// help menu
void help()
{
  printf("run <job> <time> <pri>: submit a job named <job>,\n\t\t\texecution time is <time>,\n\t\t\tpriority is <pri>.\n")
  printf("list: display the job status.")
  printf("fcfs: change the scheduling policy to FCFS.")
  printf("sjf: change the scheduling policy to SJF.")
  printf("priority: change the scheduling policy to priority.")
  printf("test <benchmark> <policy> <num_of_jobs> <priority_levels>\n\t<min_CPU_time> <max_CPU_time>")
  printf("quit: exit AUbatch")

  return;
}
