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
    int int_msg;
    char char_msg[BUFFER_SIZE];
    pid_t pid;
    int fd1[2]; // pipe 1
    int fd2[2]; // pipe 2
    // function variables
    FILE * file;
    char buffer[BUFFER_SIZE];
    char new_char;
    int count = 0;

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
      file = fopen(argv[1], "r");

      // check if file opened successfully
      if (file == NULL)
      {
        err_msg(1);
        exit(EXIT_FAILURE);
      }

      // read file data into char array
      while ((new_char = fgetc(file)) != EOF)
      {
        buffer[count] = new_char;
        count++;
      }

      // write file data to pipe1
      printf("Process 1 starts sending data to Process 2 ...\n");
      write(fd1[WRITE_END], buffer, BUFFER_SIZE);

      //read word number result from pipe2
      read(fd2[READ_END], &int_msg, sizeof(int_msg));
      printf("Process 1: the total number of words is %d \n", int_msg);
 
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
      read(fd1[READ_END], char_msg, BUFFER_SIZE);

      // count words and write to pipe2
      printf("Process 2 is counting words now...\n");
      int_msg = count_words(char_msg);
      printf("%d \n", int_msg);
      //write_msg = sprintf(write_msg, "%05d", count_result);
      printf("Process 2 is sending the result back to Process 1 ...\n");
      write(fd2[WRITE_END], &int_msg, sizeof(int_msg));
     
      // close read pipe1 and write pipe2 
      close(fd1[READ_END]);
      close(fd2[WRITE_END]);  
    }
    // close file1 and file2
    fclose(file);

    return 0;
}
