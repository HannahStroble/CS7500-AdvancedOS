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
#include <assert.h>
#include <stdbool.h>

// Local files
#include "commandline.h"
#include "schedule.h"

/* Error Code */
#define EINVAL       1
#define E2BIG        2

#define MAXMENUARGS  4 
#define MAXCMDLINE   64 

/*
 * The run command - submit a job.
 */
int cmd_run(int nargs, char **args) {
	if (nargs != 4) {
		printf("Usage: run <job> <time> <priority>\n");
		return EINVAL;
	}   
        /* Use execv to run the submitted job in AUbatch */
		err_msg("At: cmd_run before scheduler", err_flag);
		scheduler(nargs, args);
		err_msg("Finished: cmd_run after scheduler", err_flag);

		return 0; /* if succeed */
}

/*
 * The quit command.
 */
int cmd_quit(int nargs, char **args) {
	printf("Please display performance information before exiting AUbatch!\n");
  	exit(0);
}

// list processes
int process_list()
{
	// check if there are any running processes
	if ((finished_next != 0) || (p_waiting != 0))
	{
		// counter
		int i;

		// print format
		printf("Name	CPU_Time	Pri	Arrival_time	Progress");

		// iterate through finished processes
		for (i=0; i < finished_next; i++)
		{
			// define current finished process
			new_process p_finish = finished_processes[i];
			char *stat = "finished";
			char *time = p_finish->arrival_time;
			printf("%s %d %d %s %s\n",
					p_finish->program,
					p_finish->cpu_time,
					p_finish->priority,
					time, stat);
		}

		// iterate through waiting processes
		for (i=0; i < buff_next; i++)
		{
			// define current waiting processes
			new_process p_wait = running_processes[i];
			char *stat = "---waiting---";

			// check if there is remaining burst time
			if ((p_wait->cpu_first_time > 0)&&(p_wait->cpu_time_remaining > 0))
			{
				stat = "running";
			}

			// print out format
			char *time = p_wait->arrival_time;
			printf("%s %d %d %s %s\n", 
					p_wait->program,
					p_wait->cpu_time,
					p_wait->priority,
					time, stat);
		}
	}
	else
	{
		printf("No processes are running.\n");
	}

	// return 
	return 0;
}

/*
 * Display menu information
 */
void showmenu(const char *x[])
{
	// counter
	int i;

    // print out newline
	printf("\n");
	
    // print out all menu items
	for (i=0; x[i]; i++) {
		printf("%s", x[i]);
	}
    // final newline
	printf("\n");
}

// help menu struct or text
static const char *helpmenu[] = {
	"run <job> <time> <pri>: submit a job named <job>,\n\t\t\texecution time is <time>,\n\t\t\tpriority is <pri>.\n",
	"list: display the job status.\n",
	"fcfs: change the scheduling policy to FCFS.\n",
	"sjf: change the scheduling policy to SJF.\n",
	"priority: change the scheduling policy to priority.\n",
	"test <benchmark> <policy> <num_of_jobs> <priority_levels>\n     <min_CPU_time> <max_CPU_time>\n",
	"quit: exit AUbatch\n",
	NULL
};

// call and printout help menu
int cmd_helpmenu(int n, char **a)
{
	// default number of args and char array of entry to print
	(void)n;
	(void)a;

    // print helpmenu
	showmenu(helpmenu);
	return 0;
}

/*
 *  Command table.
 */
static struct {
	const char *name;
	int (*func)(int nargs, char **args);
} cmdtable[] = {
	/* commands: single command must end with \n */
	{ "?\n",	cmd_helpmenu },
	{ "h\n",	cmd_helpmenu },
	{ "help\n",	cmd_helpmenu },
	{ "r",		cmd_run },
	{ "run",	cmd_run },
	{"list",	process_list },
	{ "q\n",	cmd_quit },
	{ "quit\n",	cmd_quit },
    /* Please add more operations below. */
    {NULL, NULL}
};

/*
 * Process a single command.
 */
int cmd_dispatch(char *cmd)
{
	time_t beforesecs, aftersecs, secs;
	u_int32_t beforensecs, afternsecs, nsecs;
	char *args[MAXMENUARGS];
	int nargs=0;
	char *word;
	char *context;
 	int i, result;

	for (word = strtok_r(cmd, " ", &context);
	     word != NULL;
	     word = strtok_r(NULL, " ", &context)) {

		if (nargs >= MAXMENUARGS) {
			printf("Command line has too many words\n");
			return E2BIG;
		}
		args[nargs++] = word;
	}

	if (nargs==0) {
		return 0;
	}

	for (i=0; cmdtable[i].name; i++) {
		if (*cmdtable[i].name && !strcmp(args[0], cmdtable[i].name)) {
			assert(cmdtable[i].func!=NULL);
            
            /*Qin: Call function through the cmd_table */
			result = cmdtable[i].func(nargs, args);
			return result;
		}
	}

	printf("%s: Command not found\n", args[0]);
	return EINVAL;
}

/*
 * Command line main loop.
 */
void *commandline( void *ptr )
{
	    char *buffer;
        size_t bufsize = 64;
        
        buffer = (char*) malloc(bufsize * sizeof(char));
        if (buffer == NULL) {
 		perror("Unable to malloc buffer");
 		exit(1);
	}

    while (1) {
		printf("\n>");
		getline(&buffer, &bufsize, stdin);
		cmd_dispatch(buffer);
	}
        return 0;
}

