#include <stdio.h>
#include <unistd.h> 
#include <sys/types.h>
#include <string.h>

#include "helper.h"

#define BUFFER_SIZE 25
#define READ_END 0
#define WRITE_END 1

// main function
int main(void)
{
    err_msg(0);
    char write_msg[BUFFER_SIZE] = "Hello";
    char read_msg[BUFFER_SIZE];
    pid_t pid;
    int fd[2];

    // create pipe
    if (pipe(fd) == -1)
    {
        fprintf(stderr,"Pipe Failed");
        return 1;
    }

    // fork a child process
    pid = fork();
    if (pid < 0)
    {
        fprintf(stderr,"Fork Failed");
        return 1;
    }
    // parent process
    if (pid > 0)
    {
        
    }
    // child process
    else
    {
        
    }

    return 0;
}
