///////////////////////////////////////////////////////////////////
// COMP7500 - Project 4 - cpmFS - A Simple File System
// Name: Hannah Reinbolt
// Date: 4/13/2021
// Description: Functions to impliment a simple file system.
// Note: Referenced code provided by Dr. Qin
//
// How to run:
// >> make
// >> ./cpmRun
//
///////////////////////////////////////////////////////////////////


// libraries
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/types.h>
#include <stdbool.h>

// local libraries
#include "cpmfsys.h"



//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
///////////    REQUIRED FUNCTIONS
//////////////////////////////////////////////////////////////////////////


//function to allocate memory for a DirStructType (see above), and populate it, given a
//pointer to a buffer of memory holding the contents of disk block 0 (e), and an integer index
// which tells which extent from block zero (extent numbers start with 0) to use to make the
// DirStructType value to return. 

// init constructor
DirStructType *mkDirStruct(int index,uint8_t *e)
{
    // variables
    DirStructType *DIR;
    int i;

    // address of directory entry at block referenced by e with an index
    uint8_t *ADDRESS = e + (index * EXTENT_SIZE);

    // allocate memory
    DIR = malloc(sizeof(DirStructType));

    // failed memory allocation
    if (DIR == NULL)
    {
        printf("ERROR: Failed to allocate directory memory.\n");
        exit(1);
    }

    // status stored at location
    DIR->status = ADDRESS[0]; 

    // initilize name
    /*
    for (i = 0; i < 9; i++)
    {
        if ( i != 8) { DIR->name[i] = ' '; }
        else { DIR->name[i] = '\0'; }
    }
    */

    // add values to name
    for (i = 0; i < 8; i++)
    {
        if (ADDRESS[i+1] != ' ' && ADDRESS[i+1] == true) { DIR->name[i] = ADDRESS[i+1]; }
        else { DIR->name[i] = '\0'; break; }
    } 

    // check if name is 0
    /*
    if (strlen(DIR->name) == 0)
    { DIR->name[0] = '\0'; }
    */

    // initilize extension
    /*
    for (i = 0; i < 4; i++)
    {
        if ( i != 3) { DIR->extension[i] = ' '; }
        else { DIR->extension[i] = '\0'; }
    }
    */

    // add values to extension
    // try to use memcopy after this
    for (i = 0; i < 4; i++)
    {
        if (ADDRESS[i+9] != ' ' && ADDRESS[i+9] == true) { DIR->extension[i] = ADDRESS[i+9]; }
        else { DIR->extension[i] = '\0'; break; }
    } 

    // check if extension is 0
    /*
    if (strlen(DIR->extension) == 0)
    { DIR->extension[0] = '\0'; }
    */

    // initialize size bytes
    DIR->XL = ADDRESS[12];
    DIR->BC = ADDRESS[13];
    DIR->XH = ADDRESS[14];
    DIR->RC = ADDRESS[15];

    // initialize blocks
    for (i = 0; i < 16, i++)
    { DIR->blocks[i] = ADDRESS[i+16]; }

    // return structure
    return DIR;

} 

// function to write contents of a DirStructType struct back to the specified index of the extent
// in block of memory (disk block 0) pointed to by e

// copy constructor
void writeDirStruct(DirStructType *d, uint8_t index, uint8_t *e)
{
    // variables
    uint8_t *ADDRESS = e + (index + EXTENT_SIZE);
    int i;

    // copy file name
    for (i = 0; i < 8; i++)
    { ADDRESS[i+1] = d->name[i]; }

    // copy extension 
    for (i = 0; i < 3; i++)
    { ADDRESS[i+9] = d->extension[i]; }

    // copy size bytes
    ADDRESS[12] = d->XL;
    ADDRESS[13] = d->BC;
    ADDRESS[14] = d->XH;
    ADDRESS[15] = d->RC;

    // copy blocks
    for (i = 0; i < 16; i++)
    { ADDRESS[i+16] = d->blocks[i]; }

    // write blocks to disk
    blockWrite(e, 0);

} 

// populate the FreeList global data structure. freeList[i] == true means 
// that block i of the disk is free. block zero is never free, since it holds
// the directory. freeList[i] == false means the block is in use. 
void makeFreeList(); 
// debugging function, print out the contents of the free list in 16 rows of 16, with each 
// row prefixed by the 2-digit hex address of the first block in that row. Denote a used
// block with a *, a free block with a . 
void printFreeList(); 


// internal function, returns -1 for illegal name or name not found
// otherwise returns extent nunber 0-31
int findExtentWithName(char *name, uint8_t *block0); 

// internal function, returns true for legal name (8.3 format), false for illegal
// (name or extension too long, name blank, or  illegal characters in name or extension)
bool checkLegalName(char *name); 


// print the file directory to stdout. Each filename should be printed on its own line, 
// with the file size, in base 10, following the name and extension, with one space between
// the extension and the size. If a file does not have an extension it is acceptable to print
// the dot anyway, e.g. "myfile. 234" would indicate a file whose name was myfile, with no 
// extension and a size of 234 bytes. This function returns no error codes, since it should
// never fail unless something is seriously wrong with the disk 
void cpmDir(); 

// error codes for next five functions (not all errors apply to all 5 functions)  
/* 
    0 -- normal completion
   -1 -- source file not found
   -2 -- invalid  filename
   -3 -- dest filename already exists 
   -4 -- insufficient disk space 
*/ 

//read directory block, 
// modify the extent for file named oldName with newName, and write to the disk
int cpmRename(char *oldName, char * newName); 

// delete the file named name, and free its disk blocks in the free list 
int  cpmDelete(char * name); 



//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
///////////    EXTRA FUNCTIONS
//////////////////////////////////////////////////////////////////////////

// following functions need not be implemented for this assignment


int  cpmCopy(char *oldName, char *newName); 


int  cpmOpen( char *fileName, char mode); 

// non-zero return indicates filePointer did not point to open file 
int cpmClose(int filePointer); 

// returns number of bytes read, 0 = error 
int cpmRead(int pointer, uint8_t *buffer, int size);

// returns number of bytes written, 0 = error 
int cpmWrite(int pointer, uint8_t *buffer, int size);  

