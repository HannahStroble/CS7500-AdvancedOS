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

// public variables
int freeblocks[300] = {1, 0};



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

    // add values to name
    for (i = 0; i < 8; i++)
    {
        if (ADDRESS[i+1] != ' ' && ADDRESS[i+1] == true) { DIR->name[i] = ADDRESS[i+1]; }
        else { DIR->name[i] = '\0'; break; }
    } 

    // add values to extension
    // try to use memcopy after this
    for (i = 0; i < 4; i++)
    {
        if (ADDRESS[i+9] != ' ' && ADDRESS[i+9] == true) { DIR->extension[i] = ADDRESS[i+9]; }
        else { DIR->extension[i] = '\0'; break; }
    } 

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
void makeFreeList()
{
    // variables
    int i;

    // fully clear blank list
    for (i = 1; i < 300; i++)
    { freeblocks[i] 0; }

    // calculate real list of free blocks
    for (i = 0; i < EXTENT_SIZE; i++)
    {
        // create new dir
        DirStructType *DIR = mkDirStruct(i, NULL);

        // check if blocks are free
        if (DIR->status != 0xe5)
        {
            // check if status is empty
            for (i = 0; i < BLOCKS_PER_EXTENT; i++)
            {
                // check index
                if (DIR->blocks[i] != 0)
                { freeblocks[DIR->blocks[i]] = 1; }
            }
        }
    }
} 



// debugging function, print out the contents of the free list in 16 rows of 16, with each 
// row prefixed by the 2-digit hex address of the first block in that row. Denote a used
// block with a *, a free block with a . 
void printFreeList()
{
    // variables
    int i = 0;

    // header
    printf("\nFREE BLOCK LIST: (* means in-use)\n");

    // divide into 16x16 
    for (i = 0; i < 300; i++)
    {
        // if mod by 16
        if (i % 16 == 0)
        { printf("%3x: ", i); }

        // not free
        if (freeblocks[i] == 1)
        { printf("* "); }

        // free
        else
        { printf(". "); }

        // add newlines
        if ((i + 1) % 16 == 0)
        { printf("\n"); }
    }
}


// internal function, returns -1 for illegal name or name not found
// otherwise returns extent nunber 0-31
int findExtentWithName(char *name, uint8_t *block0)
{
    // variables
    int i;

    // check name
    if (checkLegalName(name) == true)
    {
        // split filename and extension
        char *fname = strtok(name, ".");
        char *extension = strtok(NULL, ".");

        // look through all extents
        for (i = 0; i < MAX_EXTENTS; i++)
        {
            // create a new dir type
            DirStructType *DIR = mkDirStruct(i, block0);

            // check names if status is full
            if (DIR->status != 0xe5)
            {
                // strip filename and extension of spaces
                char *strip_name = strtok(DIR->name, " ");
                char *strip_extension = strtok(DIR->extension, " ");

                // check if the names are illegal
                if (strcmp(strip_name, fname) == 0 && strcmp(strip_extension, extension) == 0)
                { return i; } 
            } // 2 if
        } // 1 for
    } // 1 if

    // illegal name found
    return -1;
} 

// internal function, returns true for legal name (8.3 format), false for illegal
// (name or extension too long, name blank, or  illegal characters in name or extension)
bool checkLegalName(char *name)
{
    // variables
    int i;

    // for all chars in name
    for (i = 0; i < strlen(name); i++)
    {
        // check punctuation
        if (isspace(name[i]) || iscntrl(name[i]) || ispunct(name[i]))
        { return false; }
    }

    // return
    return true;
} 


// print the file directory to stdout. Each filename should be printed on its own line, 
// with the file size, in base 10, following the name and extension, with one space between
// the extension and the size. If a file does not have an extension it is acceptable to print
// the dot anyway, e.g. "myfile. 234" would indicate a file whose name was myfile, with no 
// extension and a size of 234 bytes. This function returns no error codes, since it should
// never fail unless something is seriously wrong with the disk 
void cpmDir()
{
    // variables
    int i;
    int j;
    int dir_len;

    // dir list
    printf("DIRECTORY LISTING");

    // for all extents
    for (i = 0; i < EXTENT_SIZE; i++)
    {
        // make new dir
        DirStructType *DIR = mkDirStruct(i, NULL);

        // if empty blocks
        if (DIR->status != 0xe5)
        {
            // set dir length
            dir_len = 0;

            // for all blocks
            for (j = 0; j < BLOCKS_PER_EXTENT; j++)
            {
                // count if false
                if (DIR->blocks[j] != 0)
                { dir_len++; }
            }

            // last block is calculated with RC and BC
            dir_len--;
            int block_len = ((DIR->RC * 128) + DIR->BC + (dir_len * 1024));

            // print for even output
            char *filename = strtok(DIR, ".");
            char *extension = strtok(DIR, NULL);
            printf("%s.", filename);
            printf("%s-12s %d\n", extension, block_len);
        }
    }

}

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
int cpmRename(char *oldName, char * newName)
{
    // variables
    int result = 0;

    // check for illegal name
    if (checkLegalName(newName) == 0)
    { printf("Invalid filename: %s\n", newName); result = -1; }

    // check if already exists
    else if (findExtentWithName(newName, NULL) != -1)
    { printf("File already exists: %s\n", newName); result = -1; }

    // continue if good name
    else 
    {
        // find extent 
        int loc = findExtentWithName(oldName, NULL);

        // rename
        if (loc != -1)
        {
            DirStructType *DIR = mkDirStruct(loc, NULL);
            
            // get filename and extension
            char *filename = strtok(newName, ".");
            char *extension = strtok(newName, NULL);

            // copy to new dir
            strcpy(DIR->name, filename);
            strcpy(DIR->extension, extension);

            // write to disk
            writeDirStruct(DIR, loc, NULL);
            result = 0;
        }

        // file doesn't exist
        else 
        { printf("File does not exist: %s\n", oldName); result = -1; }
    }

    // return
    return result;
} 

// delete the file named name, and free its disk blocks in the free list 
int  cpmDelete(char * name)
{
    // variables
    uint8_t zblock;

    // read block at 0
    int loc = findExtentWithName(name, &zblock);

    // check if location is valid
    if (loc != -1)
    {
        // create blank dir
        DirStructType *DIR = mkDirStruct(loc, NULL);

        // set status to unused
        // this does not fully erase data ####################
        DIR->status = 0xe5;

        // write blank dir to disk
        writeDirStruct(DIR, loc, NULL);
        return 0;
    }

    // does not exist
    else
    { printf("Name does not exist: %s\n", name); return -1; }
} 



//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
///////////    EXTRA FUNCTIONS
//////////////////////////////////////////////////////////////////////////

// following functions need not be implemented for this assignment
/*

int  cpmCopy(char *oldName, char *newName); 


int  cpmOpen( char *fileName, char mode); 

// non-zero return indicates filePointer did not point to open file 
int cpmClose(int filePointer); 

// returns number of bytes read, 0 = error 
int cpmRead(int pointer, uint8_t *buffer, int size);

// returns number of bytes written, 0 = error 
int cpmWrite(int pointer, uint8_t *buffer, int size); 
*/ 

