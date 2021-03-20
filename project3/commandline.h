// helperfile

/* Error Code */
#define EINVAL       1
#define E2BIG        2

#define MAXMENUARGS  8 
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

// commandline
int cmd_quit(int nargs, char **args); 
int cmd_dispatch(char *cmd);
void *commandline( void *ptr );

// processing
int cmd_run(int nargs, char **args);
int process_list(int nargs, char **args);
int run_bench(int nargs, char **argv);

// menu
void menu_execute(char *line, int isargs); 
void showmenu(const char *x[]);
int cmd_helpmenu(int n, char **a);

// scheduling
int run_fcfs(int nargs, char **args);
int run_sjf(int nargs, char **args);
int run_pri(int nargs, char **args);