#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>

#include "helper.h"


// error messages
void err_msg(int err)
{
  // Incorrect function usage - too few or too many arguments
  if (err == 0)
  {
    printf("Please enter a file name.\n\nUsage: ./pwordcount <filename>\n\n");
  }

  return;
}

// get input from terminal
int get_input(int argc, char *argv[])
{
  char in[3000];
  if ((argc < 1) || (argc > 1))
  {
    err_msg(0);
  }
  else
  {
    
  }
}


// print function
void print(char[] str)
{
  printf(str);
  return;
}
