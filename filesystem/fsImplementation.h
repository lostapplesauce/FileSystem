// File created by Pedro Souto - This file WAS NOT given to us by the professor
// This is the file where we will implement all the functionalities professor asked
// From instructions: The driver should be interactive (with all built in commands) to list directories, create directories, add and remove files, copy files, move files, setting meta data, and two “special commands” one to copy from the normal filesystem to your filesystem and the other from your filesystem to the normal filesystem
// Header file for fsImplementation.c

#ifndef fsImplementation_h
#define fsImplementation_h
#include <stdio.h>
#endif /* fsImplementation_h */

/*
 HEADER PROTOTYPES FOR FILE SYSTEM FUNCTIONALITIES
 ALL FUNCTIONS PROTYPES WILL GO HERE
 ALL FUNCTIONS IMPLEMENTATIONS WILL GO IN .c FILE
*/

// List directories

// Create directory

// Add file

// Remove file

// Cope file

// Move file

// Set metadata

// Copy from the normal filesystem to your filesystem

// Copy from your filesystem to the normal filesystem

// Print Menu
void printMenu(void);

// Initialize control block (block 0)
int initializeVolumeControlBlock(uint64_t volumeSize, char *volumeName, uint16_t blockSize);

/*
 Check if volume control block has been initialized before
 Returns 0 if control block has not been initialized
 Returns 1 if control block has been initialized
*/
int hasVolumeControlBlock(uint16_t blockSize);

// Initialize free space information blocks (blocks 1-49)
void intializeFreeSpaceInformation(uint64_t volumeSize, int16_t blockSize);

/*
The following methods are used for setting/getting bits in the bit array
Source: http://www.mathcs.emory.edu/~cheung/Courses/255/Syllabus/1-C-intro/bit-array.html
*/
void setBit(int *A[], uint64_t bit);
void clearBit(int *A[], uint64_t bit);
int getBit(int *A[], uint64_t bit);
