///////////////////////////////////////////////////////////////
// COMP7500 - Project 2 - Pipe-based Wordcount Tool
// Name: Hannah Reinbolt
// Date: 2/9/2021
/////////////////////////////////////////////////////////////// 

// libraries
#include <stdio.h>
#include <unistd.h> 
#include <sys/types.h>
#include <string.h>
#include <stdlib.h>

// local libraries
#include "helper.h"

// local definitions
#define BUFFER_SIZE 10000
#define READ_END 0
#define WRITE_END 1

////////////////////////////////////////////////////////
// name: main
// use: create and run unix pipes to read in a file and count number of words
// description: read in file and count all words inside file using unix pipes
//
// input: int argc
// input description: count of command line arguments
// input: char *argv[]
// input description: char array of all command line arguments
// output: int
// output description: integer just to close function
////////////////////////////////////////////////////////
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

    // check if correct number of command line arguments are supplied
    if (argc < 2 || argc > 2)
    {
      // print right error message and exit
      err_msg(0);
      exit(1);
    }

    // create two pipes
    if (pipe(fd1) == -1 || pipe(fd2) == -1)
    {
        fprintf(stderr,"One of the Pipes Failed");
        return 1;
    }

    // fork both processes
    pid = fork();
    if (pid < 0)
    {
        fprintf(stderr,"Fork Failed");
        return 1;
    }

    // parent process
    // description: read in file, send to child, and recieve wordcount result using pipes
    if (pid > 0)
    {
      // close unneeded pipe1 read and pipe2 write
      close(fd1[READ_END]);
      close(fd2[WRITE_END]);

      // open file
      printf("\nProcess 1 is reading file \"%s\" now ... \n", argv[1]);
      file = fopen(argv[1], "r");

      // check if file opened successfully
      if (file == NULL)
      {
        // print correct error message and exit
        err_msg(1);
        exit(EXIT_FAILURE);
      }

      // read file data into char array
      while ((new_char = fgetc(file)) != EOF)
      {
        // add char into char array
        buffer[count] = new_char;
        count++;
      }

      // write file char array data to pipe1
      printf("\nProcess 1 starts sending data to Process 2 ...\n");
      write(fd1[WRITE_END], buffer, BUFFER_SIZE);

      //read word number result from pipe2
      read(fd2[READ_END], &int_msg, sizeof(int_msg));
      printf("\nProcess 1: The total number of words is %d. \n", int_msg);
 
      // close write pipe1 and read pipe2
      close(fd1[WRITE_END]);
      close(fd2[READ_END]);   
    }

    // child process
    // description: recieve file information from parent, count words, and 
    // send number result to parent using pipes
    else
    {
      // close unneeded pipe1 write and pipe2 read
      close(fd1[WRITE_END]);
      close(fd2[READ_END]);
 
      //recieve from pipe1
      printf("\nProcess 2 finishes receiving data from Process 1 ...\n");
      read(fd1[READ_END], char_msg, BUFFER_SIZE);

      // count words and write to pipe2
      printf("\nProcess 2 is counting words now ...\n");
      int_msg = count_words(char_msg);
      printf("\nProcess 2 is sending the result back to Process 1 ...\n");
      write(fd2[WRITE_END], &int_msg, sizeof(int_msg));
     
      // close read pipe1 and write pipe2 
      close(fd1[READ_END]);
      close(fd2[WRITE_END]);  
    }
    // close file
    fclose(file);

    // return to end program
    return 0;
}
