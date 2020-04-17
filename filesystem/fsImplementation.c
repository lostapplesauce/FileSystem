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
    printf("M. See Menu Again.\n");
    printf("E. Exit.\n");
    printf("-------------------------------------------------------\n\n");
}

void initializeVolumeControlBlock(uint64_t volumeSize, char *volumeName, uint16_t blockSize) {
    // Allocate memory for struct
    struct volumeControlBlock *vcb = malloc(blockSize);
    
    // Set correct parameters
    vcb->volumeSize = volumeSize;
    vcb->rootDirectory = 50;
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
    printf("Root Directory Block: %llu\n", vcb->rootDirectory);
    printf("VOLUME CONTROL BLOCK CREATED SUCCESSFULLY!\n");
    printf("-------------------------------------------------------\n\n");
    
    // Write VCB to LBA block 0
    LBAwrite(vcb, 1, 0);
    
    // Since this is the first time this partition is being created, we must also intialize the free space information blocks
    intializeFreeSpaceInformation(volumeSize, blockSize);
    
    // Since this is the first time this partition is being created, we must also intialize a root directory
    createRootDirectory(44, blockSize);
    
    // Cleanup
    free(vcb);
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
    fsi->lowestBlockAccessible = 100;
    fsi->highestBlockAccessible = ((volumeSize/blockSize) - 1);
    
    // Clear bits for blocks 0-49, since they are used by VCB and FSI blocks
    for (int i = 0; i < 50; i++) {
        clearBit(fsi->freeBlockBitArray, i);
    }
    
    // Print information about the newly created FSI blocks
//    printf("-------------------------------------------------------\n");
//    printf("CREATING FREE SPACE INFORMATION BLOCKS...\n");
//    printf("Free Space: %llu\n", fsi->freeSpace);
//    printf("Lowest Block Accessible: %hhu\n", fsi->lowestBlockAccessible);
//    printf("Highest Block Accessible: %llu\n", fsi->highestBlockAccessible);
//    printf("-------------------------------------------------------\n\n");
    
    // Write blocks to file system
    LBAwrite(fsi, 49, 1);
    
    // Cleanup
    free(fsi);
}

void listDirectories(uint16_t blockSize) {
    struct directoryEntry *dirs = malloc(blockSize * 49);
    LBAread(dirs, 49, 50);
    
    printf("-------------------------------------------------------\n");
    printf("Directories:\n");
    for (int i = 0; i < (49 * 8); i++) {
        // If the current directory is not empty, display it
        if (dirs[i].indexInformationLocation != 0) {
            printf("%s\n", dirs[i].name);
            //printf("Directory Index Information Location: %llu\n", dirs[i].indexInformationLocation);
        }
    }
    printf("-------------------------------------------------------\n\n");
    
    // Cleanup
    free (dirs);
}

void createDirectory(char* directoryName, uint16_t permissions, uint16_t blockSize) {
    // TODO: IMPLEMENT
}

void createRootDirectory(uint16_t permissions, uint16_t blockSize) {
    // Since we know the root directory will be in block 50, we only need to pull that single block from the LBA
    struct directoryEntry *dirs = malloc(blockSize);
    LBAread(dirs, 1, 50);
    
    // Create temp directory, which will be written to file system
    struct directoryEntry *rootDir = malloc(sizeof(struct directoryEntry));
    
    // Set variables for the root directory
    strcpy(rootDir->name, "ROOT");
    strcpy(rootDir->fileExtension, DIRECTORY_EXTENSION_NAME);
    rootDir->permissions = permissions;
    rootDir->dateCreated = (unsigned int)time(NULL);
    rootDir->fileSize = 0;
    
    // Find an empty block for the index information
    uint64_t indexLocation = findSingleFreeLBABlockInRange(100, getHighestUseableBlock(blockSize), blockSize);
    if (indexLocation == 0) {
        printf("ERROR AT createRootDirectory(): NO SPACE FOUND FOR INDEX INFORMATION BLOCK!\n");
        exit(1);
    }
    
    // Create the indexInformation Structure
    struct indexInformation *ii = malloc(blockSize);
    
    // Set information for index information to empty, since hen root directory is created, indexInformation has no information that is relevant
    //ii->nextFileIndexLocation = 0;
    //ii->fileIndexLocation = 0;
    memset(ii, 0x00, sizeof(struct indexInformation));
    
    // Assign the index struct location and set that block to used
    rootDir->indexInformationLocation = indexLocation;
    setBlockAsUsed(indexLocation, blockSize);
    
    // Copy this new root directory to the 0th element
    memcpy(&(dirs[0]), rootDir, sizeof(struct directoryEntry));
    
    // Write back the block
    LBAwrite(dirs, 1, 50);
    
    //Cleanup
    free(dirs);
    free(rootDir);
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

void* findFreeLBABlocksInRange(uint64_t startIndex, uint64_t endIndex, int numBlocksNeeded, int16_t blockSize) {
    // Get FSI struct
    struct freeSpaceInformation *fsi = malloc(49 * blockSize);
    LBAread(fsi, 49, 1);
    
    // Pointer to array of free blocks
    uint64_t *blocks = malloc(numBlocksNeeded * sizeof(int));
    
    // Set number of blocks found to 0
    int numBlocksFound = 0;
    
    // Visit every block and see its status
    for (uint64_t i = startIndex; i <= endIndex; i++) {
        // If we have found all the free blocks we need, then return those blocks as an array pointer
        if (numBlocksFound == numBlocksNeeded) {
            free(fsi);
            return blocks;
        }
        else {
            // If the current block we are checking is free, then add it to the free list
            if (getBit(fsi->freeBlockBitArray, i) == 1) {
                blocks[numBlocksFound] = i;
                numBlocksFound++;
            }
        }
    }
    
    // If we did not find enough blocks, then cleanup and return a null pointer
    free(fsi);
    free(blocks);
    return NULL;
}

uint64_t findSingleFreeLBABlockInRange(uint64_t startIndex, uint64_t endIndex, int16_t blockSize) {
    // Get FSI struct
    struct freeSpaceInformation *fsi = malloc(49 * blockSize);
    LBAread(fsi, 49, 1);
    
    // Visit every block and see its status
    for (uint64_t i = startIndex; i <= endIndex; i++) {
        // If the current block we are checking is free, then return its block number
        if (getBit(fsi->freeBlockBitArray, i) == 1) {
            return i;
        }
    }
    
    // If no free block was found, cleanup and return 0
    free(fsi);
    return 0;
}

void setBlockAsUsed(uint64_t blockNumber, int16_t blockSize) {
    // Get FSI struct
    struct freeSpaceInformation *fsi = malloc(49 * blockSize);
    LBAread(fsi, 49, 1);
    
    // Clear the bit to 0 (Used)
    clearBit(fsi->freeBlockBitArray, blockNumber);
    
    //Cleanup
    free(fsi);
}

uint64_t getHighestUseableBlock(int16_t blockSize) {
    // Get FSI struct
    struct freeSpaceInformation *fsi = malloc(49 * blockSize);
    LBAread(fsi, 49, 1);
    
    // Save highest block to return after cleanup
    uint64_t highestBlock = fsi->highestBlockAccessible;
    
    // Cleanup
    free(fsi);
    
    return highestBlock;
}
