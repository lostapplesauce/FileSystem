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
    sampleCreateDirectories(blockSize);
    printCommands();
    while (1) {
        // Prompt user for functionality choice
        char input[128];
        fgets(input, 128, stdin);
        
        // Remove trailing new line from input (\n)
        // Source: https://stackoverflow.com/questions/2693776/removing-trailing-newline-character-from-fgets-input
        input[strcspn(input, "\n")] = 0;
        
        // ls commands
        if (strcmp(input, "ls") == 0) {
            listDirectories(getVCBRootDirectory(blockSize), blockSize);
        }
        
        // menu command
        else if ((strcmp(input, "m") == 0) || (strcmp(input, "M") == 0) || (strcmp(input, "Menu") == 0) || (strcmp(input, "menu") == 0)) {
            printCommands();
        }
        
        // exit command
        else if ((strcmp(input, "e") == 0) || (strcmp(input, "E") == 0) || (strcmp(input, "Exit") == 0) || (strcmp(input, "exit") == 0)) {
            printf("Exiting File System...\n");
            closePartitionSystem();
            exit(0);
        }
    }
}

