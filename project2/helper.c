//////////////////////////////////////////////////////////////
// COMP7500 - Project 2 - Pipe-based Wordcount Tool
// Name: Hannah Reinbolt
// Date: 2/9/2021
// Description: Helper functions for main pwordcount.c file.
//////////////////////////////////////////////////////////////

// libraries
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>

// local libraries
#include "helper.h"

////////////////////////////////////////////////
// name: err_msg
// use: error message printer
// description: quickly print different error messages for certain outcomes.
//
// input: int err
// input description: integer number representing specific error message
// output: void
// output description: no output expected
////////////////////////////////////////////////
void err_msg(int err)
{
  // incorrect function usage - too few or too many arguments
  if (err == 0)
  {
    printf("\nPlease enter a file name.\n\nUsage: ./pwordcount <filename>\n");
  }

  // improper use of pwordcount function, expect one argument only
  if (err == 1)
  {
    printf("\nUnable to open file.\n");
    printf("Please check if file exists and you have read privilege.");
  }

  // return nothing
  return;
}

//////////////////////////////////////////////////
// name: count_words
// use: count all words in a given char array
// description: count all words in a given char array
//
// input: char* arr
// input description: char array containing some chars
// output: int
// output description: integer representing number of words in char array
//////////////////////////////////////////////////
int count_words(char* arr)
  {
    // local variables
    char words = 0;
    int i;
    
    // loop through char array till end
    for (i=0; i<strlen(arr); i++)
    {
      
      // check for spaces, if so then word
      if (arr[i] == ' ' ||
          arr[i] == '\t' ||
          arr[i] == '\n' ||
          arr[i] == '\0')
      {
        words++;
      }
    }

    // return total number of words in file
    return words;
  }


