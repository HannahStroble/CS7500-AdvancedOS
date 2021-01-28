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
