#include <stdio.h>
#include <unistd.h> 
#include <sys/types.h>
#include <string.h>
#include <stdlib.h>

#include "helper.h"

#define BUFFER_SIZE 1000
#define READ_END 0
#define WRITE_END 1

// main function
int main(int argc, char *argv[])
{
    // main variables
    char write_msg[BUFFER_SIZE];
    char read_msg[BUFFER_SIZE];
    FILE * file;
    int words;
    pid_t pid;
    int fd[2];

    // open file
    file = fopen(argv[1], "r");
    
    // check if the file has opened successfully
    if (file == NULL)
    {
        err_msg(1);
        exit(EXIT_FAILURE);
    }
    
    int num = count_words(file);
    printf("%d", num);
    
    /*
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
    */

    return 0;
}
