// helperfile

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