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

/*
 * The run command - submit a job.
 */
int cmd_run(int nargs, char **args) {
	if (nargs != 4) {
		printf("Usage: run <job> <time> <priority>\n");
		return EINVAL;
	}   

	// make sure the file exists
	FILE *file1 = fopen(args[1], "r");
	if (file1 == NULL)
	{
		printf("File does not exist. Please enter valid file path and name.\n");
		return EINVAL;
	}

	// close file, we were just checking
	fclose(file1);

    /* Use execv to run the submitted job in AUbatch */
	err_msg("At: cmd_run before scheduler", err_flag);
	scheduler(nargs, args);
	err_msg("Finished: cmd_run after scheduler", err_flag);

	return 0; /* if succeed */
}

//////////////////////////////////////////////
//// METRICS

int run_bench(int nargs, char **argv)
{
	printf("run bench: srand0");
	srand(0);
	printf("args %d\n", nargs);
	// check to make sure correct function usage
	if (nargs != 7)
	{
		// print error
		printf("Usage: test <benchmark> <policy> <num_of_jobs> <arrival_rate> <priority_levels> <min_CPU_time> <max_cpu_time>\n");
		return EINVAL;
	}
	else if (p_waiting || finished_next)
	{
		printf("Processes are running on CPU, no jobs should be running if doing a benchmark.\n");
		return EINVAL;
	}
	printf("run bench: after init checks");

	// collect variables for benchmark
	char *bench = argv[1];
	char *pol = argv[2];
	int n_j = atoi(argv[3]);
	int a_rate = 0;
	int p_lvl = atoi(argv[4]);
	int minc = atoi(argv[5]);
	int maxc = atoi(argv[6]);

	printf("printing vars: bench %s pol %s job %d plvl %d minc %d maxc %d\n", bench, pol, n_j, p_lvl, minc, maxc );

	printf("run bench: after vars");

	// make sure min is not bigger than max
	if (minc > maxc)
	{
		printf("Min CPU time cannot be bigger than Max CPU time.\n");
		return EINVAL;
	}
	// make sure all variables are not negative
	else if (n_j < 0 || minc < 0 || maxc < 0 || p_lvl < 0 || a_rate < 0)
	{
		printf("Initial benchmark variables cannot be less than zero.\n");
		return EINVAL;
	}
	printf("POLICY %s\n", pol);
	// set policy
	set_policy(pol);

	printf("run bench: after policy");

	// run benchmark
	run_benchmark(bench, n_j, a_rate, p_lvl, minc, maxc);
	printf("run bench: ran bench");
	while(p_waiting){};

	printf("run bench: after while crocadile\n");
	// print metrics
	performance_metrics();
	printf("run bench: after metrics");

	// clear out list for later processes
	int k;
	for (k=0; k < finished_next; k++)
	{
		free(finished_processes[k]);
	}
	printf("run bench: after freeup\n");
	finished_next = 0;
	buff_next = 0;
	buff_prev = 0;

	// return 
	return 0;

}


////////////////////////////////////////////
////////////////////////////////////////////
//////////REPORTING
/*
 * The quit command.
 */
int cmd_quit(int nargs, char **args) {
	
	// print metrics and exit
	performance_metrics(); 
	exit(0);
}

// list processes
int process_list(int nargs, char **args)
{
	if (p_waiting > 0)
	{
		// counter
		int i;
		char *j_status;
		struct tm time2;
		char* curr_policy = get_policy(); 

		// print format
		printf("Total number of jobs in the queue: %d\n", buff_next - buff_prev);
		printf("Scheduling Policy: %s\n", curr_policy);
		printf("Name\tCPU_Time\tPri\tArrival_time\tProgress\n");

		// iterate through waiting processes
		for (i=0; i < buff_next; i++)
		{
			// define current waiting processes
			j_status = "";

			if (running_processes[i]->cpu_time_remaining == 0)
			{
				continue;
			}
			else if (running_processes[i]->cpu_first_time > 0 && running_processes[i]->cpu_time_remaining > 0)
			{
				j_status = "Run";
			}

			// print out format 
			time2 = *localtime(&running_processes[i]->arrival_time);
			
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
	{ "list\n",	process_list },
	{ "l\n",	process_list },
	{ "fcfs\n",	run_fcfs },
	{ "sjf\n",	run_sjf },
	{ "priority\n",	run_pri },
	{ "test",	run_bench },
	{ "q\n",	cmd_quit },
	{ "quit\n",	cmd_quit },
    {NULL, NULL}
};

//////////////////////////////////////////////////////////
//////// SCHEDULING

// sort running process list in fcfs 
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

	// return
	return 0;
}

// sort running process list in sjf 
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

	// return
	return 0;
}

// sort running process list in priority 
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

	// return
	return 0;
}


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

		if (nargs > MAXMENUARGS+2) {
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

