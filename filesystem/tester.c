// Required C libraries
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

// Bierman Files
#include "fsLow.h"

// Group Files
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
    
    // For DEVELOPMENT. Will be removed later ~~
    remove(argv[1]);
    
    // Start Partition System
    printf("-------------------------------------------------------\n");
    printf("OPENING FILE SYSTEM...\n");
    char * filename = argv[1];
    uint64_t volumeSize = atoll (argv[2]);
    uint64_t blockSize = atoll (argv[3]);
    int startPartitionReturn = startPartitionSystem (filename, &volumeSize, &blockSize);

    // Make sure partition was successfully create/opened
    if (startPartitionReturn == 0) {
        printf("FILE SYSTEM OPENED SUCSESSFULLY!\n");
    } else {
        printf("ERROR OPENING FILE SYSTEM!\n");
        exit(1);
    }
    printf("-------------------------------------------------------\n\n");
    
    // If a Volume Control Block has not been created before, create it now
    if (!hasVolumeControlBlock(blockSize)) {
        // This will create the Volume Control Block AND it will also initialize the Free Space Information blocks
        initializeVolumeControlBlock(volumeSize, PARTITION_NAME, blockSize);
    }
    
    
    
    // Main loop of program, where we ask for user input then carry out that functionality
    while (0) {
        // Prompt user for functionality choice
        char userInput;
        printMenu();
        userInput = getchar();
        
        // Valid that user input is a digit between 0-9 inclusive
        if (!isdigit(userInput) || !(userInput >= '0' && userInput <='9')) {
            printf("Invalid choice. Please enter a different option.\n");
        }
        
        if (userInput == '0') {
            // Close partition and exit the program
            printf("Exiting File System...\n");
            closePartitionSystem();
            exit(0);
        }
    }
}

