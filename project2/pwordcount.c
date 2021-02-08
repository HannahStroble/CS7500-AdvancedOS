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
    // pipe variables
    char write_msg[BUFFER_SIZE];
    char read_msg[BUFFER_SIZE];
    pid_t pid;
    int fd1[2]; // pipe 1
    int fd2[2]; // pipe 2
    // function variables
    FILE * file;
    int words;
    
    
    // create pipe
    if (pipe(fd1) == -1 || pipe(fd2) == -1)
    {
        fprintf(stderr,"One of the Pipes Failed");
        return 1;
    }

    // fork a process
    pid = fork();
    if (pid < 0)
    {
        fprintf(stderr,"Fork Failed");
        return 1;
    }
    // parent process
    if (pid > 0)
    {
      // close unneeded pipe1 read and pipe2 write
      close(fd1[READ_END]);
      close(fd2[WRITE_END]);

      // open file
      printf("Process 1 is reading file %s now ... \n", argv[0]);
      file = fopen(argv[1], "r");

      // check if file opened successfully
      if (file == NULL)
      {
        err_msg(1);
        exit(EXIT_FAILURE);
      }

      // send file data to child process
      printf("Process 1 starts sending data to Process 2 ...\n");
      write(fd1[WRITE_END], file, BUFFER_SIZE);
      //wait(3);
      read(fd2[READ_END], read_msg, BUFFER_SIZE);
      printf("Process 1: the total number of words is %d \n", read_msg);
      //close(fd1[WRITE_END]);
      //close(fd2[READ_END]);   
    }
    // child process
    else
    {
      // close unneeded pipe1 write and pipe2 read
      close(fd1[WRITE_END]);
      close(fd2[READ_END]);
 
      //printf("inside child\n");
      printf("Process 2 finishes receiving data from Process 1...\n");
      read(fd1[READ_END], file, BUFFER_SIZE);
      printf("Process 2 is counting words now...\n");
      //sprintf(write_msg, "%ld", count_words(file));
      printf("Process 2 is sending the result back to Process 1 ...\n");
      write(fd2[WRITE_END], "6", BUFFER_SIZE);
      //printf("child read %s\n", read_msg);
      //close(fd1[READ_END]);
      //close(fd2[WRITE_END]);  
    }
    fclose(file);   

    return 0;
}
