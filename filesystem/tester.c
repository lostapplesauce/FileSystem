#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <errno.h>
#include <math.h>
#include<ctype.h>

#include "fsLow.h"
#include "fsStructures.h"
#include "fsImplementation.h"

#define PARTITION_NAME "Pedro's HDD"

int main (int argc, char *argv[]) {
    // Ensure the correct number of arguments
    // argv[0]: Program Name
    // argv[1]: File Name
    // argv[2]: Volume Size
    // argv[3]: Block Size
    if (argc != 4) {
        printf("In correct number of parameters. Please use the following parameters:\n");
        printf("argv[1]: File Name\n");
        printf("argv[2]: Volume Size\n");
        printf("argv[3]: Block Size\n");
        exit(1);
    }
    
    // For DEV. Will be removed later
    remove(argv[1]);
    
    // Start Partition System
    printf("-------------------------------------------------------\n");
    printf("OPENING FILE SYSTEM...\n");
    char * filename = argv[1];
    uint64_t volumeSize = atoll (argv[2]);
    uint64_t blockSize = atoll (argv[3]);
    int startPartitionReturn = startPartitionSystem (filename, &volumeSize, &blockSize);

    // Make sure partition was successfull
    if (startPartitionReturn == 0) {
        printf("FILE SYSTEM OPENED SUCSESSFULLY!\n");
    } else {
        printf("ERROR OPENING FILE SYSTEM!\n");
        exit(1);
    }
    printf("-------------------------------------------------------\n\n");
    
    // Check if a volumeControlBlock has already been created
    if (!hasVolumeControlBlock(blockSize)) {
        char *vn = PARTITION_NAME;
        initializeVolumeControlBlock(volumeSize, vn, blockSize);
    }
    
    
    closePartitionSystem();
    exit(0);
    
    
    // Main loop of program, where we ask for user input then carry out that functionality
    while (1) {
        // Prompt user for functionality choice
        char userInput;
        printMenu();
        userInput = getchar();
        
        // Valid that user input is a digit between 0-9 inclusive
        if (!isdigit(userInput) || !(userInput >= '0' && userInput <='9')) {
            printf("Invalid choice. Please enter a different option.\n");
        }
        
        if (userInput == '0') {
            printf("Exiting File System...\n");
            exit(0);
        }
    }
}

