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

// Local files
#include "commandline.h"

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
        printf("use execv to run the job in AUbatch.\n");
      	return 0; /* if succeed */
}

/*
 * The quit command.
 */
int cmd_quit(int nargs, char **args) {
	printf("Please display performance information before exiting AUbatch!\n");
  	exit(0);
}

/*
 * Display menu information
 */
void showmenu(const char *name, const char *x[])
{
	int ct, half, i;

	printf("\n");
	printf("%s\n", name);
	
	for (i=ct=0; x[i]; i++) {
		ct++;
	}
	half = (ct+1)/2;

	for (i=0; i<half; i++) {
		printf("    %-36s", x[i]);
		if (i+half < ct) {
			printf("%s", x[i+half]);
		}
		printf("\n");
	}

	printf("\n");
}

static const char *helpmenu[] = {
	"[run] <job> <time> <priority>       ",
	"[quit] Exit AUbatch                 ",
	"[help] Print help menu              ",
        /* Please add more menu options below */
	NULL
};

int cmd_helpmenu(int n, char **a)
{
	(void)n;
	(void)a;

	showmenu("AUbatch help menu", helpmenu);
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


