// File created by Pedro Souto - This file WAS NOT given to us by the professor
// This is the file where we will implement all the functionalities professor asked
// From instructions: The driver should be interactive (with all built in commands) to list directories, create directories, add and remove files, copy files, move files, setting meta data, and two “special commands” one to copy from the normal filesystem to your filesystem and the other from your filesystem to the normal filesystem

// Required C libraries
#include <stdlib.h>
#include <string.h>
#include <time.h>

// Bierman Files
#include "fsLow.h"

// Group Files
#include "fsStructures.h"
#include "fsImplementation.h"

void printMenu(){
    printf("-------------------------------------------------------\n");
    printf("Please choose one of the following options:\n");
    printf("1. List Directories.\n");
    printf("2. Create Directoty.\n");
    printf("3. Add File.\n");
    printf("4. Remove File.\n");
    printf("5. Copy File.\n");
    printf("6. Move File.\n");
    printf("7. Set Metadata.\n");
    printf("8. Copy from the normal filesystem to this filesystem.\n");
    printf("9. Copy from this filesystem to the normal filesystem.\n");
    printf("0. Exit.\n");
    printf("-------------------------------------------------------\n\n");
}

int initializeVolumeControlBlock(uint64_t volumeSize, char *volumeName, uint16_t blockSize) {
    // Allocate memory for struct
    struct volumeControlBlock *vcb = malloc(blockSize);
    
    // Set correct parameters
    vcb->volumeSize = volumeSize;
    strcpy(vcb->volumeName, volumeName);
    vcb->blockSize = blockSize;
    vcb-> numBlocks = (volumeSize / blockSize);

    // Generate and set a random volumeID
    srand((unsigned int)time(0));
    uint32_t ID = rand();
    vcb->volumeID = ID;
    
    // Print information about the newly created VCB
    printf("-------------------------------------------------------\n");
    printf("CREATING VOLUME CONTROL BLOCK...\n");
    printf("Volume Name: %s\n", vcb->volumeName);
    printf("Volume ID: %u\n", vcb->volumeID);
    printf("Volume Size: %llu\n", vcb->volumeSize);
    printf("Volume LBA Block Size: %llu\n", vcb->blockSize);
    printf("Number of LBA Blocks: %llu\n", vcb->numBlocks);
    printf("VOLUME CONTROL BLOCK CREATED SUCCESSFULLY!\n");
    printf("-------------------------------------------------------\n\n");
    
    // Write VCB to LBA block 0
    LBAwrite(vcb, 1, 0);
    
    // Since this is the first time this partition is being created, we must also intialize the free space information blocks
    intializeFreeSpaceInformation(volumeSize, blockSize);
    
    // Cleanup
    free(vcb);
    
    // Success
    return 0;
}

int hasVolumeControlBlock(uint16_t blockSize) {
    // Create a temp volumeControlBlock to gather back information from block 0
    struct volumeControlBlock *vcb = malloc(blockSize);

    // Read from LBA block 0
    LBAread(vcb, 1, 0);
    
    // If the VCB variables have a default of 0, the VCB has not been created yet
    if (vcb->volumeID == 0 && vcb->volumeSize == 0 && vcb->blockSize == 0) {
        //Cleanup
        free (vcb);
        
        //Since the VCB does not exist, print 0
        return 0;
    } else {
        // Since the VCB does exist, print information about it
        printf("-------------------------------------------------------\n");
        printf("VOLUME CONTROL BLOCK ALREADY CREATED.\n");
        printf("Volume Name: %s\n", vcb->volumeName);
        printf("Volume ID: %u\n", vcb->volumeID);
        printf("Volume Size: %llu\n", vcb->volumeSize);
        printf("Volume LBA Block Size: %llu\n", vcb->blockSize);
        printf("Number of LBA Blocks: %llu\n", vcb->numBlocks);
        printf("-------------------------------------------------------\n\n");
        
        // **** Also print information about the VCB, since it exists ****
        struct freeSpaceInformation *fsi = getFreeSpaceInformation(blockSize);
        printf("-------------------------------------------------------\n");
        printf("FREE SPACE INFORMATION BLOCKS ALREADY CREATED.\n");
        printf("Free Space: %llu\n", fsi->freeSpace);
        printf("Lowest Block Accessible: %hhu\n", fsi->lowestBlockAccessible);
        printf("Highest Block Accessible: %llu\n", fsi->highestBlockAccessible);
        printf("-------------------------------------------------------\n\n");
        
        //Cleanup
        free(vcb);
        free(fsi);
        
        // Since the VCB exists, print 1
        return 1;
    }
}

void intializeFreeSpaceInformation(uint64_t volumeSize, int16_t blockSize) {
    // LBA Block 0: Volume Control Block
    // LBA Block 1-49: Free Space Information Blocks
    // Total Blocks used by system: 50
    uint32_t numUsedBytes = (50 * blockSize);
    
    // Actual volume usuable, after VCB and FSI blocks are taken into account
    uint64_t actualVolumeSize = volumeSize - numUsedBytes;
    
    // Create struct in order to later write it to the file system
    struct freeSpaceInformation *fsi = malloc(blockSize * 49);
    
    // Set all bits to 1, since at the beggining, they are all free
    memset(fsi, 0xFF, (blockSize * 49));
    
    // Set other parameters
    fsi->freeSpace = actualVolumeSize;
    fsi->lowestBlockAccessible = 50;
    fsi->highestBlockAccessible = ((volumeSize/blockSize) - 1);
    
    // Clear bits for blocks 0-49, since they are used by VCB and FSI blocks
    for (int i = 0; i < 50; i++) {
        clearBit(fsi->freeBlockBitArray, i);
    }
    
    // Print information about the newly created FSI blocks
    printf("-------------------------------------------------------\n");
    printf("CREATING FREE SPACE INFORMATION BLOCKS...\n");
    printf("Free Space: %llu\n", fsi->freeSpace);
    printf("Lowest Block Accessible: %hhu\n", fsi->lowestBlockAccessible);
    printf("Highest Block Accessible: %llu\n", fsi->highestBlockAccessible);
    printf("-------------------------------------------------------\n\n");
    
    // Write blocks to file system
    LBAwrite(fsi, 49, 1);
    
    // Cleanup
    free(fsi);
}

void setBit(int A[], uint64_t bit) {
    uint64_t i = bit / 32;          // i = array index (use: A[i])
    uint64_t pos = bit % 32;        // pos = bit position in A[i]

    unsigned int flag = 1;          // flag = 0000.....00001

    flag = flag << pos;             // flag = 0000...010...000   (shifted k positions)

    A[i] = A[i] | flag;             // Set the bit at the k-th position in A[i]
}

void clearBit(int A[], uint64_t bit) {
    uint64_t i = bit / 32;
    uint64_t pos = bit % 32;

    unsigned int flag = 1;          // flag = 0000.....00001

    flag = flag << pos;             // flag = 0000...010...000   (shifted k positions)
    flag = ~flag;                   // flag = 1111...101..111

    A[i] = A[i] & flag;             // RESET the bit at the k-th position in A[i]
}

int getBit(int A[], uint64_t bit) {
    uint64_t i = bit / 32;
    uint64_t pos = bit % 32;

    unsigned int flag = 1;          // flag = 0000.....00001

    flag = flag << pos;             // flag = 0000...010...000   (shifted k positions)

    if ( A[i] & flag )              // Test the bit at the k-th position in A[i]
        return 1;
    else
        return 0;
}

void* getFreeSpaceInformation(int16_t blockSize) {
    struct freeSpaceInformation *fsi = malloc(49 * blockSize);
    LBAread(fsi, 49, 1);
    return fsi;
}
