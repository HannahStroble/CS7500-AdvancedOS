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
  if (err == 1)
  {
    printf("\nUnable to open file.\n");
    printf("Please check if file exists and you have read privilege.");
  }

  return;
}


// count all words in a file
int count_words(char* arr)
  {
    char words = 0;
    int i;
    
    // loop through file till end
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


