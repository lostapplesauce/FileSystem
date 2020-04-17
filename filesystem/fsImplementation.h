/*
 File created by Pedro Souto - This file WAS NOT given to us by the professor
 This is the file where we will implement all the functionalities listed in the instructions
 From instructions: The driver should be interactive (with all built in commands) to list directories, create directories, add and remove files, copy files, move files, setting meta data, and two “special commands” one to copy from the normal filesystem to your filesystem and the other from your filesystem to the normal filesystem
 */

#ifndef fsImplementation_h
#define fsImplementation_h
#include <stdio.h>
#endif /* fsImplementation_h */

#define DIRECTORY_EXTENSION_NAME "DIRECTORY"

// List directories
void listDirectories(uint16_t blockSize);

// Create directory
void createDirectory(char* directoryName, uint16_t permissions, uint16_t blockSize);

// Create the root directory
// Should not call this function, it is auto called when volume control block is created
void createRootDirectory(uint16_t permissions, uint16_t blockSize);

// Add file

// Remove file

// Copy file

// Move file

// Set metadata

// Copy from the normal filesystem to your filesystem

// Copy from your filesystem to the normal filesystem

// Print Menu
void printMenu(void);

// Initialize control block (block 0)
void initializeVolumeControlBlock(uint64_t volumeSize, char *volumeName, uint16_t blockSize);

// Check if volume control block has been initialized before
// Returns 0 if control block has not been initialized
// Returns 1 if control block has been initialized
int hasVolumeControlBlock(uint16_t blockSize);

// Initialize free space information blocks (blocks 1-49)
void intializeFreeSpaceInformation(uint64_t volumeSize, int16_t blockSize);

// Get free space information struct
// Returns a pointer to the FreeSpaceInformation struct
void* getFreeSpaceInformation(int16_t blockSize);

// Sets the Kth bit in array A - Sets to 1 (Free)
// Source: http://www.mathcs.emory.edu/~cheung/Courses/255/Syllabus/1-C-intro/bit-array.html
void setBit(int A[], uint64_t bit);

// Clears the Kth bit in array A -  sets to 0 (Used)
// Source: http://www.mathcs.emory.edu/~cheung/Courses/255/Syllabus/1-C-intro/bit-array.html
void clearBit(int A[], uint64_t bit);

// Returns the Kth bit in array A
// Source: http://www.mathcs.emory.edu/~cheung/Courses/255/Syllabus/1-C-intro/bit-array.html
int getBit(int A[], uint64_t bit);

// Finds free blocks in a given range. This is used when you want MULTIPLE blocks
// Returns a pointer to an array of ints, which represent the blocks that are free
void* findFreeLBABlocksInRange(uint64_t startIndex, uint64_t endIndex, int numBlocksNeeded, int16_t blockSize);

// Finds a single free blocks in a given range. This is used when you want ONLY ONE blocks
// Returns a single int to the free block
uint64_t findSingleFreeLBABlockInRange(uint64_t startIndex, uint64_t endIndex, int16_t blockSize);

// Set a single block as used. This is used when you want to set ONLY ONE block to used
void setBlockAsUsed(uint64_t blockNumber, int16_t blockSize);

// Returns the highest useable block in the LBA. Block may be USED or NOT USED
uint64_t getHighestUseableBlock(int16_t blockSize);
