//////////////////////////////////////
// COMP7500 - Project 3 - A Pthread-based Batch Scheduling System
// Name: Hannah Reinbolt
// Date: 3/14/2021
// Description: Commandline user interface global variables
// Note: Referenced code provided by Dr. Qin
//////////////////////////////////////////

// errors
#define EINVAL       1
#define E2BIG        2

// commandline
#define MAXMENUARGS  8 
#define MAXCMDLINE   64 

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