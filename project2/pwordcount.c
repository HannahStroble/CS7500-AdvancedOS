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
    int write_msg;
    int read_msg;
    pid_t pid;
    int fd1[2]; // pipe 1
    int fd2[2]; // pipe 2
    // function variables
    FILE * file1;
    char buffer[BUFFER_SIZE];
    int count_result;
    
    file1 = fopen(argv[1], "r");
    file_to_char(file1, BUFFER_SIZE);    

    /*
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
      printf("Process 1 is reading file %s now ... \n", argv[1]);
      file1 = fopen(argv[1], "r");
      buffer = file1;
      printf("%s", buffer);

      // check if file opened successfully
      if (file1 == NULL)
      {
        err_msg(1);
        exit(EXIT_FAILURE);
      }

      // write file data to pipe1
      printf("Process 1 starts sending data to Process 2 ...\n");
      write(fd1[WRITE_END], file1, strlen(file1));

      //read word number result from pipe2
      read(fd2[READ_END], &read_msg, sizeof(read_msg));
      printf("Process 1: the total number of words is %d \n", read_msg);
 
      // close write pipe1 and read pipe2
      close(fd1[WRITE_END]);
      close(fd2[READ_END]);   
    }

    // child process
    else
    {
      // close unneeded pipe1 write and pipe2 read
      close(fd1[WRITE_END]);
      close(fd2[READ_END]);
 
      //recieve from pipe1
      printf("Process 2 finishes receiving data from Process 1...\n");
      read(fd1[READ_END], file2, strlen(file2));

      // count words and write to pipe2
      printf("Process 2 is counting words now...\n");
      write_msg = count_words(file2);
      printf("%d", write_msg);
      //write_msg = sprintf(write_msg, "%05d", count_result);
      printf("Process 2 is sending the result back to Process 1 ...\n");
      write(fd2[WRITE_END], &write_msg, sizeof(write_msg));
     
      // close read pipe1 and write pipe2 
      close(fd1[READ_END]);
      close(fd2[WRITE_END]);  
    }
    // close file1 and file2
    fclose(file1);
    fclose(file2);*/   

    return 0;
}
