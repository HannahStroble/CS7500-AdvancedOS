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
int count_words(FILE *input)
  {
    char words = 0;
    char new_char;
    
    // loop through file till end
    while ((new_char = fgetc(input)) != EOF)
    {
      // check for spaces, if so then word
      if (new_char == ' ' ||
          new_char == '\t' ||
          new_char == '\n' ||
          new_char == '\0')
      {
        words++;
      }
    }

    // return total number of words in file
    return words;
  }


// convert file into char array
char * file_to_char(FILE * file, int size)
  {
    char result[size] = "";
    char c;

    while((c=fgetc(file)) != EOF )
    {
      strcat(result, c);
    }
    return result;

  }




