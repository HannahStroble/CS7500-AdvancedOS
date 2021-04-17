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
#include <limits.h>
#include <ctype.h>

// local libraries
#include "cpmfsys.h"

// public variables
bool freeblocks[NUM_BLOCKS];



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
    char item;
    int i;

    // allocate memory for dir
    DirStructType *DIR = malloc(sizeof(DirStructType));

    // make dir address
    uint8_t *ADDRESS = e + (index * EXTENT_SIZE);

    // failed memory allocation
    if (DIR == NULL)
    { printf("ERROR: Failed to allocate directory memory.\n"); exit(1); }
    
    // set initial status
    DIR->status = ADDRESS[0]; 

    // set initial name 
    for (i = 1; i < 9; i++)
    {
        item = ADDRESS[i];
        if (item != ' ') { DIR->name[i - 1] = item; }
        else { DIR->name[i - 1] = '\0'; break; }
    }

    // set end null for name
    DIR->name[8] = '\0';

    // set initial extension
    for (i = 9; i < 12; i++)
    {
        item = ADDRESS[i];
        if (item != ' ') { DIR->extension[i - 9] = item; }
        else { DIR->extension[i - 9] = '\0'; break; }
    }

    // set end null for extension
    DIR->extension[3] = '\0'; 

    // set initial size bytes
    DIR->XL = ADDRESS[12];
    DIR->BC = ADDRESS[13];
    DIR->XH = ADDRESS[14];
    DIR->RC = ADDRESS[15];

    // set initial blocks
    for (i = 16; i < 32; i++)
    { DIR->blocks[i - 16] = ADDRESS[i]; }

    // return new directory
    return DIR;
} 

// function to write contents of a DirStructType struct back to the specified index of the extent
// in block of memory (disk block 0) pointed to by e

// copy constructor
void writeDirStruct(DirStructType *d, uint8_t index, uint8_t *e)
{
    // variables 
    uint8_t item;  // uint8_t and char hold the same data type
    int i;

    // make dir address
    uint8_t *ADDRESS = e + (index * EXTENT_SIZE);
    
    // set initial status
    ADDRESS[0] = d->status; 

    // set initial name 
    for (i = 1; i < 9; i++)
    {
        item = d->name[i - 1];
        if (item != '\0') { ADDRESS[i] = item; }
        else { ADDRESS[i] = ' '; }
    }

    // set initial extension
    for (i = 9; i < 12; i++)
    {
        item = d->extension[i - 9];
        if (item != '\0') { ADDRESS[i] = item; }
        else { ADDRESS[i] = ' '; }
    }

    // set initial size bytes
    ADDRESS[12] = d->XL;
    ADDRESS[13] = d->BC;
    ADDRESS[14] = d->XH;
    ADDRESS[15] = d->RC;

    // set initial blocks
    for (i = 16; i < 32; i++)
    { ADDRESS[i] = d->blocks[i - 16]; }

    // write to disk and free list
    blockWrite(e, 0);
    makeFreeList();
} 

// populate the FreeList global data structure. freeList[i] == true means 
// that block i of the disk is free. block zero is never free, since it holds
// the directory. freeList[i] == false means the block is in use. 
void makeFreeList()
{
    // variables
    int i;
    int k;

    // re-allocate and set array to full
    uint8_t *block0 = malloc(BLOCK_SIZE);

    // fully clear blank list
    for (i = 0; i < BLOCK_SIZE; i++)
    { freeblocks[i] = 1; }

    // set dir block to false
    freeblocks[0] = 0;

    // read from disk
    blockRead(block0, 0);

    // calculate real list of free blocks
    for (i = 0; i < EXTENT_SIZE; i++)
    {
        // create new dir based on block0
        DirStructType *DIR = mkDirStruct(i, block0);

        // check if blocks are free
        if (DIR->status != 0xe5)
        {
            // check if status is empty
            for (k = 0; k < BLOCKS_PER_EXTENT; k++)
            {
                // check index
                if (DIR->blocks[k] != 0)
                { freeblocks[DIR->blocks[k]] = 0; }
            }
        }

        // free dir pointer memory
        free(DIR);
    }

    // free block0 memory
    free(block0);
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
    for (i = 0; i < NUM_BLOCKS; i++)
    {
        // if mod by 16
        if (i % 16 == 0)
        { printf("%3x: ", i); }

        // not free
        if (freeblocks[i] == 0)
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
    char *cpyname = malloc(strlen(name));
    char *cpyextension = malloc(strlen(name));

    // check if name is legal then free memory
    if (0 > checkName(name, cpyname, cpyextension))
    { free(cpyextension); free(cpyname); return -1; }

    // iterate through all blocks
    for (i = 0; i < EXTENT_SIZE; i++)
    {
        // create new dir
        DirStructType *DIR = mkDirStruct(i, block0);

        // check if extension and name are legal
        if ((DIR->status != 0xe5) &&
             strcmp(DIR->name, cpyname) == 0 &&
             strcmp(DIR->extension, cpyextension) == 0)
        { free(DIR), free(cpyname), free(cpyextension); return i; }
    }

    // illegal found
    return -1;
} 

// internal function, returns true for legal name (8.3 format), false for illegal
// (name or extension too long, name blank, or  illegal characters in name or extension)
bool checkLegalName(char *name)
{
    // variables
    int i;

    // check if name exists and is alphanumeric
    if (strlen(name) > 0 && isalnum(name[0]) == 0)
    { return false; }

    // for all chars in name
    for (i = 0; i < strlen(name); i++)
    {
        // check if a space, control char, punctuation, or number
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
    int block_len;
    int file_len;

    // re-init block0
    uint8_t *block0 = malloc(BLOCK_SIZE);

    // read from disk
    blockRead(block0, 0);

    // dir list
    printf("DIRECTORY LISTING");

    // for all extents
    for (i = 0; i < EXTENT_SIZE; i++)
    {
        // make new dir
        DirStructType *DIR = mkDirStruct(i, block0);

        // if empty blocks
        if (DIR->status != 0xe5)
        {
            // set block length
            block_len = 0;

            // for all blocks
            for (j = 0; j < BLOCKS_PER_EXTENT; j++)
            {
                // count if false
                if (DIR->blocks[j] != 0)
                { block_len++; }
            }

            // last block is calculated with RC and BC
            file_len = ((DIR->RC * 128) + DIR->BC + ((block_len - 1) * 1024));

            // print file stats
            printf("%s.%s %d\n", DIR->name, DIR->extension, file_len);
        }
        // free pointer
        free(DIR);
    }
    // free pointer
    free(block0);

}

//read directory block, 
// modify the extent for file named oldName with newName, and write to the disk
int cpmRename(char *oldName, char * newName)
{
    // variables
    char *cpyname = malloc(strlen(newName));
    char *cpyextension = malloc(strlen(newName));
    int e_index = -1;

    // re-init block0
    uint8_t *block0 = malloc(BLOCK_SIZE);

    // check if name is legal and then free
    if (0 > checkName(newName, cpyname, cpyextension))
    { free(cpyname); free(block0); free(cpyextension); return -1; }

    // read from disk
    blockRead(block0, 0);

    // extent index
    e_index = findExtentWithName(oldName, block0);

    // check if extention is free
    if (0 > e_index)
    { free(cpyname); free(block0); free(cpyextension); return -1; }

    // create new dir
    DirStructType *DIR = mkDirStruct(e_index, block0);

    // copy finalized name and extention
    strcpy(DIR->name, cpyname);
    strcpy(DIR->extension, cpyextension);

    // create new dir in correct place with new info
    writeDirStruct(DIR, e_index, block0);

    // free the memory
    free(DIR); free(block0); free(cpyname); free(cpyextension);

    // return 
    return 0;
} 

// delete the file named name, and free its disk blocks in the free list 
int  cpmDelete(char * name)
{
    // variables
    int e_index = -1;

    // re-initialize block0
    uint8_t *block0 = malloc(BLOCK_SIZE);

    // read from disk
    blockRead(block0, 0);

    // read block at 0
    e_index = findExtentWithName(name, block0);

    // check if file is valid
    if (e_index < 0)
    { printf("Name does not exist: %s\n", name); return -1; }

    // create blank dir at location block0
    DirStructType *DIR = mkDirStruct(e_index, block0);

    // set status to unused
    DIR->status = 0xe5;

    // write blank dir to disk
    writeDirStruct(DIR, e_index, block0);

    // free pointers
    free(block0);
    free(DIR);

    // return false
    return 0;

} 



//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
///////////    HELPER FUNCTIONS
//////////////////////////////////////////////////////////////////////////

// help check name and extention
int checkName(char *name, char *newname, char *extension)
{
    // variables
    bool name_len_check = true;
    int e_index = 0;
    int n_index = 0;
    int i;

    // check name and extension
    for (i = 0; i < strlen(name); i++)
    {
        // check for valid name
        if (name[i] != '.' && name_len_check == 1)
        { newname[i] = name[i]; }

        // check name length
        else if (name_len_check == 1)
        { name_len_check = 0; n_index = i; }

        // check if name length is false
        else if (name_len_check == 0)
        { extension[e_index] = name[i]; e_index; }
    }

    // reset name index
    if (n_index == 0)
    { n_index = strlen(name); }

    // fill name with empties
    for (i = n_index; i < 9; i++)
    { newname[i] = '\0'; }

    // fill extention with empties
    for (i = e_index; i < 3; i++)
    { extension[i] = '\0'; }

    // check filename and extension for errors
    if (strlen(newname) == 0  ||
        strlen(newname) > 8   ||
        strlen(extension) > 3 ||
        checkLegalName(newname) == false ||
        checkLegalName(extension) == false )
    { return -1; }

    // return 
    return 0;
}

