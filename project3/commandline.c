//////////////////////////////////////
// COMP7500 - Project 3 - A Pthread-based Batch Scheduling System
// Name: Hannah Reinbolt
// Date: 3/14/2021
// Description: Commandline user interface
// Note: Referenced code provided by Dr. Qin
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


//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
///////////    HELP, QUIT and LIST FUNCTIONS
//////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////
// name: cmd_quit
// use: quit user interface terminal
// description: exit out of the user interface terminal
//
// input: int nargs
// input description: number of arguments in args array
// input: char **args
// input description: char array of arguments
// output: int
// output description: dummy value for the commandline processor
////////////////////////////////////////////////
int cmd_quit(int nargs, char **args) {
	
	// print metrics and exit
	performance_metrics(0,0); 
	exit(0);
}


////////////////////////////////////////////////
// name: process_list
// use: print running and waiting processes
// description: print currently running and waiting processes
//
// input: int nargs
// input description: number of arguments in args array
// input: char **args
// input description: char array of arguments
// output: int
// output description: dummy value for the commandline processor
////////////////////////////////////////////////
int process_list(int nargs, char **args)
{
	// while there are processes waiting
	if (p_waiting > 0)
	{
		// counters
		int i;
		char *j_status;
		struct tm time2;

		// get string policy
		char* curr_policy = get_policy(); 

		// print statistics
		printf("Total number of jobs in the queue: %d\n", buff_next - buff_prev);
		printf("Scheduling Policy: %s\n", curr_policy);
		printf("Name\tCPU_Time\tPri\tArrival_time\tProgress\n");

		// iterate through waiting processes
		for (i=0; i < buff_next; i++)
		{
			// define current waiting processes
			j_status = "";

			// skip if the processes has finished executing
			if (running_processes[i]->cpu_time_remaining == 0)
			{
				continue;
			}
			// mark as running process
			else if (running_processes[i]->cpu_first_time > 0 && running_processes[i]->cpu_time_remaining > 0)
			{
				j_status = "Run";
			}

			// current time
			time2 = *localtime(&running_processes[i]->arrival_time);
			
			// print processes
			printf("%s\t%d\t\t%d\t%02d:%02d:%02d\t%s\t\n", 
			running_processes[i]->job_name, 
			running_processes[i]->cpu_time, 
			running_processes[i]->priority, 
			time2.tm_hour,
			time2.tm_min,
			time2.tm_sec,
			j_status);
		}
	}

	// else no processes are running
	else
	{
		printf("No processes are running.\n");
	}

	// exit
	return 0;
}


////////////////////////////////////////////////
// name: helpmenu
// use: printout for help menu
// description: printout for the help menu called by cmd_helpmenu
////////////////////////////////////////////////
static const char *helpmenu[] = {
	"run <job> <time> <pri>: submit a job named <job>,\n\t\t\texecution time is <time>,\n\t\t\tpriority is <pri>.\n",
	"list: display the job status.\n",
	"fcfs: change the scheduling policy to FCFS.\n",
	"sjf: change the scheduling policy to SJF.\n",
	"priority: change the scheduling policy to priority.\n",
	"test <benchmark> <policy> <num_of_jobs> <arrival_rate>\n     <priority_levels> <min_CPU_time> <max_CPU_time>\n",
	"quit: exit AUbatch\n",
	NULL
};


////////////////////////////////////////////////
// name: showmenu
// use: print help menu
// description: print help menu
//
// input: const char *x
// input description: char array of characters to print
// output: void
// output description: nothing
////////////////////////////////////////////////
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


////////////////////////////////////////////////
// name: cmd_helpmenu
// use: call print help menu command
// description: read in user input and print help menu
//
// input: int n
// input description: number of arguments in args array
// input: char **a
// input description: char array of arguments
// output: int
// output description: dummy value for the commandline processor
////////////////////////////////////////////////
int cmd_helpmenu(int n, char **a)
{
	// default number of args and char array of entry to print
	(void)n;
	(void)a;

    // print helpmenu
	showmenu(helpmenu);
	return 0;
}


//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////      SCHEDULING FUNCTIONS
//////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////
// name: run_fcfs
// use: switch scheduling policy to First Come First Serve
// description: switch scheduling policy and re-sort list of waiting processes
//
// input: int nargs
// input description: number of arguments in args array
// input: char **args
// input description: char array of arguments
// output: int
// output description: dummy value for the commandline processor
////////////////////////////////////////////////
int run_fcfs(int nargs, char **args)
{
	// switch policy and print
	policy = fcfs;
	printf("Scheduling policy is switched to FCFS. All the %d waiting jobs have been rescheduled.\n", (buff_next - buff_prev)-1);

	// sort if there are processes
	if (p_waiting)
	{
		sort_process_list(running_processes);
	}

	// exit
	return 0;
}


////////////////////////////////////////////////
// name: run_sjf
// use: switch scheduling policy to Shortest Job First
// description: switch scheduling policy and re-sort list of waiting processes
//
// input: int nargs
// input description: number of arguments in args array
// input: char **args
// input description: char array of arguments
// output: int
// output description: dummy value for the commandline processor
////////////////////////////////////////////////
int run_sjf(int nargs, char **args)
{
	// switch policy and print
	policy = sjf;
	printf("Scheduling policy is switched to SJF. All the %d waiting jobs have been rescheduled.\n", (buff_next - buff_prev)-1);

	// sort if there are processes
	if (p_waiting)
	{
		sort_process_list(running_processes);
	}

	// exit
	return 0;
}


////////////////////////////////////////////////
// name: run_pri
// use: switch scheduling policy to Priority Sort
// description: switch scheduling policy and re-sort list of waiting processes
//
// input: int nargs
// input description: number of arguments in args array
// input: char **args
// input description: char array of arguments
// output: int
// output description: dummy value for the commandline processor
////////////////////////////////////////////////
int run_pri(int nargs, char **args)
{
	// switch policy and print
	policy = priority;
	printf("Scheduling policy is switched to PRIORITY. All the %d waiting jobs have been rescheduled.\n", (buff_next - buff_prev)-1);

	// sort if there are processes
	if (p_waiting)
	{
		sort_process_list(running_processes);
	}

	// exit
	return 0;
}


//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
////////////    BENCHMARK FUNCTIONS
//////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////
// name: run_bench
// use: run multiple processes automatically
// description: test algorithm and process efficiency automatically
//
// input: int nargs
// input description: number of arguments in args array
// input: char **args
// input description: char array of arguments
// output: int
// output description: dummy value for the commandline processor
////////////////////////////////////////////////
int run_bench(int nargs, char **argv)
{
	// check to make sure correct number of args are sent
	if (nargs != 8)
	{
		// print error
		printf("Usage: test <benchmark> <policy> <num_of_jobs> <arrival_rate> <priority_levels> <min_CPU_time> <max_cpu_time>\n");
		return EINVAL;
	}
	// make sure no other jobs are running
	else if (p_waiting)
	{
		printf("Processes are running on CPU, no jobs should be running if doing a benchmark.\n");
		return EINVAL;
	}

	// collect variables for benchmark process
	char *bench_name = argv[1];
	char *policy_name = argv[2];
	int num_jobs = atoi(argv[3]);
	int arrival_rate = atoi(argv[4]);
	int priority_lvl = atoi(argv[5]);
	int minc = atoi(argv[6]);
	int maxc = atoi(argv[7]);

	// make sure min is not bigger than max
	if ((minc >= maxc) || (num_jobs <= 0 || minc < 0 || maxc < 0 || priority_lvl < 0 || arrival_rate < 0))
	{
		printf("Min CPU time cannot be bigger than or equal to Max CPU time.\n");
		printf("Initial benchmark variables cannot be less than zero.\n");
		return EINVAL;
	}
	
	// set policy
	int policy_result = set_policy(policy_name);

	// return 0 if failed
	if (policy_result == 1)
	{
		return EINVAL;
	}

	// run benchmark
	run_benchmark(bench_name, num_jobs, arrival_rate, priority_lvl, minc, maxc);

	// wait till nothing is processing anymore
	while(p_waiting) { }

	// print metrics
	performance_metrics(num_jobs, 1);

	// exit 
	return 0;
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//////    COMMANDLINE and PROCESS EXECUTION FUNCTIONS        
////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////
// name: cmd_run
// use: send new processes to the process scheduler
// description: send new processes to the process scheduler and add to queue
//
// input: int nargs
// input description: number of arguments in args array
// input: char **args
// input description: char array of arguments
// output: int
// output description: dummy value for the commandline processor
////////////////////////////////////////////////
int cmd_run(int nargs, char **args) {

	// check for correct number of arguments
	if (nargs != 4) {
		printf("Usage: run <job> <time> <priority>\n");
		return EINVAL;
	}   

	// make sure the file exists
	FILE *file1 = fopen(args[1], "r");
	if (file1 == NULL)
	{
		// exit if doesn't exist
		printf("File does not exist. Please enter valid file path and name.\n");
		return EINVAL;
	}

	// close file, we were just checking
	fclose(file1);

    // send file to process scheduler to create new process and add to process queue
	scheduler(nargs, args);

	// if success
	return 0;
}


////////////////////////////////////////////////
// name: cmdtable
// use: organize all the commandline commands
// description: so the dispatch function can quickly associate user input with real commands
////////////////////////////////////////////////
static struct {
	const char *name;
	int (*func)(int nargs, char **args);
} cmdtable[] = {
	// commands: single command must end with \n 
	{ "?\n",	cmd_helpmenu },
	{ "h\n",	cmd_helpmenu },
	{ "help\n",	cmd_helpmenu },
	{ "r",		cmd_run },
	{ "run",	cmd_run },
	{ "list\n",	process_list },
	{ "l\n",	process_list },
	{ "fcfs\n",	run_fcfs },
	{ "sjf\n",	run_sjf },
	{ "priority\n",	run_pri },
	{ "test",	run_bench },
	{ "q\n",	cmd_quit },
	{ "quit\n",	cmd_quit },
	{ "quit -i\n",	cmd_quit },	
	{ "quit -d\n",	cmd_quit },
    {NULL, NULL}
};


////////////////////////////////////////////////
// name: cmd_dispatch
// use: process a single command from the commandline
// description: process or execute a command from the commandline menu
//
// input: char *cmd
// input description: name of command to execute
// output: int
// output description: dummy value for the commandline processor
////////////////////////////////////////////////
int cmd_dispatch(char *cmd)
{
	// local variables
	char *args[MAXMENUARGS];
	int nargs=0;
	char *word;
	char *context;
 	int i, result;

	// keep reading till empty
	for (word = strtok_r(cmd, " ", &context);
	     word != NULL;
	     word = strtok_r(NULL, " ", &context)) {

		// don't go over static argument limit
		if (nargs > MAXMENUARGS) {
			printf("Command line has too many words\n");
			return E2BIG;
		}
		args[nargs++] = word;
	}

	// if done then return
	if (nargs==0) {
		return 0;
	}

	// for each command called
	for (i=0; cmdtable[i].name; i++) {
		
		// send all arguments to specified function
		if (*cmdtable[i].name && !strcmp(args[0], cmdtable[i].name)) {
			assert(cmdtable[i].func!=NULL);
			
            // call function through the cmd_table 
			result = cmdtable[i].func(nargs, args);

			//return result
			return result;
		}
	}

	// command not found if have not returned yet
	printf("%s: Command not found\n", args[0]);
	return EINVAL;
}


////////////////////////////////////////////////
// name: commandline
// use: commandline to execute processes
// description: commandline that executes, lists and tests processes
//
// input: void *ptr
// input description: a void pointer
// output: void
// output description: returns nothing or zero
////////////////////////////////////////////////
void *commandline( void *ptr )
{
		// local variables
	    char *buffer;
        size_t bufsize = 64;
        
		// allocate memory for user input string
        buffer = (char*) malloc(bufsize * sizeof(char));

		// if failed then exit
        if (buffer == NULL) 
		{
 			perror("Unable to malloc buffer");
 			exit(1);
		}

		// forever ask for input
    	while (1) 
		{
			// get user input and send to dispatch for processing
			printf("\n>");
			getline(&buffer, &bufsize, stdin);
			cmd_dispatch(buffer);
		}

		// exit
        return 0;
}

