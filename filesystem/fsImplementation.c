// File created by Pedro Souto - This file WAS NOT given to us by the professor
// This is the file where we will implement all the functionalities professor asked
// From instructions: The driver should be interactive (with all built in commands) to list directories, create directories, add and remove files, copy files, move files, setting meta data, and two “special commands” one to copy from the normal filesystem to your filesystem and the other from your filesystem to the normal filesystem

#include <stdlib.h>
#include <string.h>
#include<time.h>

#include "fsImplementation.h"
#include "fsLow.h"
#include "fsStructures.h"

// List directories

// Create directory

// Add file

// Remove file

// Cope file

// Move file

// Set metadata

// Copy from the normal filesystem to your filesystem

// Copy from your filesystem to the normal filesystem

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

    // Generate random volumeID
    srand((unsigned int)time(NULL));
    uint32_t ID = rand();
    vcb->volumeID = ID;
    
    printf("-------------------------------------------------------\n");
    printf("CREATING VOLUME CONTROL BLOCK...\n");
    printf("Volume Name: %s\n", vcb->volumeName);
    printf("Volume ID: %u\n", vcb->volumeID);
    printf("Volume Size: %llu\n", vcb->volumeSize);
    printf("Volume LBA Block Size: %llu\n", vcb->blockSize);
    printf("VOLUME CONTROL BLOCK CREATED SUCCESSFULLY!\n");
    printf("-------------------------------------------------------\n\n");
    
    // Write volumeControlBlock to LBA element 0
    LBAwrite(vcb, 1, 0);
    
    // Cleanup
    free(vcb);
    
    // Success
    return 0;
}

int hasVolumeControlBlock(uint16_t blockSize) {
    // Create a temp volumeControlBlock to gather back information from block
    struct volumeControlBlock *vcb = malloc(blockSize);

    // Read from LBA block 0
    LBAread(vcb, 1, 0);
    
    // If the volume control block has an ID of 0, it means that it was not intialized
    if (vcb->volumeID == 0 && vcb->volumeSize == 0 && vcb->blockSize == 0) {
        printf("-------------------------------------------------------\n");
        printf("NO VOLUME CONTROL BLOCK IN THIS PARTITION.\n");
        printf("-------------------------------------------------------\n\n");
        return 0;
    }
    printf("-------------------------------------------------------\n");
    printf("VOLUME CONTROL BLOCK ALREADY CREATED IN THIS PARTITION.\n");
    printf("Volume Name: %s\n", vcb->volumeName);
    printf("Volume ID: %u\n", vcb->volumeID);
    printf("Volume Size: %llu\n", vcb->volumeSize);
    printf("Volume LBA Block Size: %llu\n", vcb->blockSize);
    printf("-------------------------------------------------------\n\n");
    return 1;
}
