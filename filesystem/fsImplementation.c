/**************************************************************
* Class: CSC-415-0# Spring 2020
* Group Name: Last Minute
* Name: Pedro Souto
* Student ID: 918412864
* Name:
* Student ID:
* Name:
* Student ID:
* Name:
* Student ID:
*
* Project: Assignment 3 – File System
* File: fsImplementation.c
*
* Description: Implementation of all functionalities the file system uses.
* **************************************************************/

// Required C libraries
#include <stdlib.h>
#include <string.h>
#include <time.h>

// Bierman Files
#include "fsLow.h"

// Group Files
#include "fsStructures.h"
#include "fsImplementation.h"

void printCommands(){
    printf("-------------------------------------------------------\n");
    printf("COMMANDS:\n");
    printf("List Directories: 'ls'\n");
    printf("Change Directories: 'cd <directory name>' or 'cd ..'\n");
    printf("Create Directoty: 'mkdir <directory name>'\n");
    printf("Add File: 'mkfile <file name> <file extension>'\n");
    printf("Remove File: 'rmfile <file name>\n");
    printf("Copy File: 'cpyfile <original file name> <new file name>'\n");
    printf("Move File: 'mvfile <file name> <directory location>'\n");
    printf("Set Metadata: 'setdata <file name> <file permission>'\n");
    printf("Copy from the normal filesystem to this filesystem: 'TODO!'\n");
    printf("Copy from this filesystem to the normal filesystem: 'TODO'\n");
    printf("See Menu Again: 'menu' or 'm'\n");
    printf("Exit: 'exit' or 'e'\n");
    printf("-------------------------------------------------------\n\n");
}

void initializeVolumeControlBlock(uint64_t volumeSize, char *volumeName, uint16_t blockSize) {
    // Allocate memory for struct
    struct volumeControlBlock *vcb = malloc(blockSize);
    
    // Set correct parameters
    vcb->volumeSize = volumeSize;
    vcb->rootDirectory = 50;
    vcb->currentDirectory = 50;
    vcb->numberOfDirectories = 0;
    strcpy(vcb->volumeName, volumeName);
    vcb->blockSize = blockSize;
    vcb-> numberOfBlocks = (volumeSize / blockSize);

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
    printf("Number of Directories: %llu\n", vcb->numberOfDirectories);
    printf("Volume LBA Block Size: %llu\n", vcb->blockSize);
    printf("Number of LBA Blocks: %llu\n", vcb->numberOfBlocks);
    printf("Root Directory Block: %llu\n", vcb->rootDirectory);
    printf("VOLUME CONTROL BLOCK CREATED SUCCESSFULLY!\n");
    printf("-------------------------------------------------------\n\n");
    
    // Write VCB to LBA block 0
    LBAwrite(vcb, 1, 0);
    
    // Since this is the first time this partition is being created, we must also intialize the free space information blocks
    intializeFreeSpaceInformation(volumeSize, blockSize);
    
    // Since this is the first time this partition is being created, we must also intialize a root directory
    createRootDirectory(444, blockSize);
    
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
        
        //Since the VCB does not exist, return 0
        return 0;
    } else {
        // Since the VCB does exist, print information about it
        printf("-------------------------------------------------------\n");
        printf("VOLUME CONTROL BLOCK ALREADY CREATED.\n");
        printf("Volume Name: %s\n", vcb->volumeName);
        printf("Volume ID: %u\n", vcb->volumeID);
        printf("Volume Size: %llu\n", vcb->volumeSize);
        printf("Number of Directories: %llu\n", vcb->numberOfDirectories);
        printf("Volume LBA Block Size: %llu\n", vcb->blockSize);
        printf("Number of LBA Blocks: %llu\n", vcb->numberOfBlocks);
        printf("Root Directory Block: %llu\n", vcb->rootDirectory);
        printf("-------------------------------------------------------\n\n");
        
        // Since the FSI exists as well, print information about it
        struct freeSpaceInformation *fsi = getFreeSpaceInformation(blockSize);
        printf("-------------------------------------------------------\n");
        printf("FREE SPACE INFORMATION ALREADY CREATED.\n");
        printf("Free Space Available: %llu\n", fsi->freeSpace);
        printf("Lowest Block Accessible: %u\n", fsi->lowestBlockAccessible);
        printf("Highest Block Accessible: %llu\n", fsi->highestBlockAccessible);
        printf("-------------------------------------------------------\n\n");
        
        
        //Cleanup
        free(vcb);
        free(fsi);
        
        // Since the VCB exists, return 1
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
    for (int i = 0; i <= 50; i++) {
        clearBit(fsi->freeBlockBitArray, i);
    }
    
    // Print info
    printf("-------------------------------------------------------\n");
    printf("CREATING FREE SPACE INFORMATION...\n");
    printf("Free Space Available: %llu\n", fsi->freeSpace);
    printf("Lowest Block Accessible: %u\n", fsi->lowestBlockAccessible);
    printf("Highest Block Accessible Size: %llu\n", fsi->highestBlockAccessible);
    printf("First Free Block: 100\n");
    printf("-------------------------------------------------------\n\n");
    
    // Write blocks to file system
    LBAwrite(fsi, 49, 1);
    
    // Cleanup
    free(fsi);
}

void* getBlockNumbersOfAllDirectories(uint16_t blockSize) {
    // Get all directories entries from the LBA
    struct directoryEntry *dirs = malloc(blockSize * 49);
    LBAread(dirs, 49, 50);
    
    // Number of directories on the file system
    uint64_t numberOfDirectories = getVCBDirectoryCount(blockSize);
    
    // Create an array to store the block numbers. We know the size by looking at the total number of directories on the file system
    uint64_t *blockArray = malloc(sizeof(uint64_t) * numberOfDirectories);
    
    // Keeps track of the number of directories we have found. This is useful to know which element in the above array we should add the block to once we have found it
    int directoriesFound = 0;

    // Iterate through all the directories
    for (int i = 0; i <= numberOfDirectories; i++) {
        // A directory has a file extension string set to the global const DIRECTORY_EXTENSION_NAME
        // If the current element we are checking is not a directory, skip it
        if (strcmp(dirs[i].fileExtension, DIRECTORY_EXTENSION_NAME) != 0) {
            continue;
        }
        
        // If the current element we are checking IS A valid directory, add it to the directory list
        else {
            blockArray[directoriesFound] = dirs[i].blockLocation;
            directoriesFound = directoriesFound + 1;
        }
        
        // If we have found all the directories, do not keep searching. Instead, jump out of loop to return the array
        if (directoriesFound == numberOfDirectories) {
            break;
        }
    }
    
    //Clean up
    free(dirs);
    
    // After we go find all the directory block locations, return the pointer to the array of blocks
    // ** THE CALLER OF THIS FUNCTION MUST FREE() THE POINTER AT SOME POINT **
    return blockArray;
}

void* getAllDirectoriesStructs(uint16_t blockSize) {
    // Get all directories entries from the LBA
    struct directoryEntry *dirs = malloc(blockSize * 49);
    LBAread(dirs, 49, 50);
    
    // Number of directories on the file system
    uint64_t numberOfDirectories = getVCBDirectoryCount(blockSize);
    
    // Create an array to store the directoryEntry structs. We know the size by looking at the total number of directories on the file system
    struct directoryEntry *dirList= malloc(sizeof(struct directoryEntry) * numberOfDirectories);
    
    // Keeps track of the number of directories we have found. This is useful to know which element in the above array we should add the struct to once we have found it
    int directoriesFound = 0;
    
    // Iterate through all the directories
    for (int i = 0; i <= numberOfDirectories; i++) {
        // A directory has a file extension string set to the global const DIRECTORY_EXTENSION_NAME
        // If the current element we are checking is not a directory, skip it
        if (strcmp(dirs[i].fileExtension, DIRECTORY_EXTENSION_NAME) != 0) {
            continue;
        }
        
        // If the current element we are checking IS A valid directory, add it to the directory list
        else {
            //dirList[directoriesFound] = dirs[i];
            memcpy(&dirList[directoriesFound], &dirs[i], sizeof(struct directoryEntry));
            directoriesFound = directoriesFound + 1;
        }
        
        // If we have found all the directories, do not keep searching. Instead, jump out of loop to return the array
        if (directoriesFound == numberOfDirectories) {
            break;
        }
    }
    
    // Cleanup
    free (dirs);
    
    // After we go find all the directory entries, return the pointer to the array of structs
    // ** THE CALLER OF THIS FUNCTION MUST FREE() THE POINTER AT SOME POINT **
    return dirList;
}

void* getDirectoryEntryFromBlock(uint64_t directoryBlockNumber, uint16_t blockSize) {
    // Since we know the location of the directory, we just need to read a single block
    struct directoryEntry *dirs = malloc(blockSize);
    LBAread(dirs, 1, directoryBlockNumber);
    
    // Ensure that this is an actual valid directory
    // A valid directory has a file extension string set to the global const DIRECTORY_EXTENSION_NAME
    if (strcmp(dirs->fileExtension, DIRECTORY_EXTENSION_NAME) == 0) {
        // Return the pointer to the struct
        // * THE CALLER IS RESPONSIBLE FOR CALLING FREE() ON THIS STRUCT *
        return dirs;
    }
    
    // If it is not valid, throw an erroe
    else {
        // Cleanup
        free (dirs);
        printf("ERROR AT getDirectoryEntryFromBlock(): TRIED TO ACCESS AN INVALID DIRECTORY BLOCK!\n");
        return NULL;
    }
}

void listDirectories (uint64_t parentDirectoryBlockNumber, uint16_t blockSize) {
    printf("-------------------------------------------------------\n");
    printf("LISTING DIRECTORIES...\n");
    listDirectoriesRecursiveHelper(parentDirectoryBlockNumber, 0, blockSize);
    printf("-------------------------------------------------------\n\n");
}

void listDirectoriesRecursiveHelper (uint64_t parentDirectoryBlockNumber, int directoryLevel, uint16_t blockSize) {
    // Since we know the location of the directory, we just need to read a single block
    struct directoryEntry *dirs = malloc(blockSize);
    LBAread(dirs, 1, parentDirectoryBlockNumber);
    
    // In order to make it look like a tree, we need to print spaces for deeper directories
    // These spaces are a function of the level we start are on. For instance, the root is at level 0, so it have no spaces.
    for (int i = 0; i < directoryLevel; i++) {
        printf("    ");
    }
    
    // Print the current directory
    printf("-%s\n", dirs->name);
    
    // We have to use recursion to print out the children, which in turn will print out its children
    // This will ensure that we keep the correct tree structure
    // If the current directory we are printing DOES HAVE children, we print them
    if (dirs->indexLocations[0]  != 0) {
        // Go to every child and print its children. Stop this process when you come across an empty child block
        for (int i = 0; dirs->indexLocations[i]  != 0; i++) {
            int childreLevel = (directoryLevel + 1);
            listDirectoriesRecursiveHelper(dirs->indexLocations[i], childreLevel, blockSize);
        }
    }
    
    free(dirs);
}

uint64_t createDirectoryOLD(uint16_t blockSize) {
    // Print Info
    printf("-------------------------------------------------------\n");
    printf("Creating Directory...\n");
    printf("Please Enter a Directory Name. No Spaces Allowed: ");
    
    // Get name for new directory. See directoryEntry struct for size of directory/file name (its 29...)
    char userInput[29];
    char *argList[29];
    char *token;
    fgets(userInput, 29, stdin);
    
    // Ensure that there was no spaces in the name
    int numberOfWords = 0;
    token = strtok(userInput, " ");
    while (token != NULL) {
        argList[numberOfWords] = token;
        numberOfWords++;
        token = strtok(NULL, " ");
    }
    
    // If there was one word only, then the name was valid and we can procced to create a new directory
    if (numberOfWords == 1) {
        // Get the block of the directory we are in
        uint64_t currentDirectoryBlock = getVCBCurrentDirectory(blockSize);
        
        // Get the directory struct of the directory we are in
        struct directoryEntry *currentDir = getDirectoryEntryFromBlock(currentDirectoryBlock, blockSize);
        
        // Get the block of the parent directory. We need this for createDirectoryHelper()
        uint64_t parentDirectoryBlock = currentDir->parentDirectory;
        
        // Create the directory
        uint64_t locationOfNewDirectory = createDirectory(argList[0], parentDirectoryBlock, blockSize);
        
        //Cleanup
        free (currentDir);
        return locationOfNewDirectory;
    }
    
    // If there was more than one word, then the name was invalid
    else {
        printf("Invalid Directoy Name. No Spaces Allowed!\n");
        printf("Failed to Create New Directory.\n");
        printf("-------------------------------------------------------\n");
        
        // Return block 0, since the creation was unsuccessfull
        return 0;
    }
}

uint64_t createDirectory(char* directoryName, uint64_t parentDirectoryBlockNumber, uint16_t blockSize) {
    // Create temp directory, which will be written to file system
    struct directoryEntry *tempDir = malloc(blockSize);
    
    // Set variables for the root directory
    strcpy(tempDir->name, directoryName);
    strcpy(tempDir->fileExtension, DIRECTORY_EXTENSION_NAME);
    tempDir->permissions = 755; // Default permission
    tempDir->dateCreated = (unsigned int)time(NULL);
    tempDir->dateModified = (unsigned int)time(NULL);
    tempDir->fileSize = 0;
    tempDir->parentDirectory = parentDirectoryBlockNumber;
    
    // Find open block to write this directory to
    uint64_t freeBlock = findSingleFreeLBABlockInRange(51, 99, blockSize);
    
    // Set this directory block location to the newly found free block
    tempDir->blockLocation = freeBlock;
    
    // Since the root has no files/children directories when created, set these pointers to 0
    memset(tempDir->indexLocations, 0x00, (sizeof(tempDir->indexLocations)/sizeof(tempDir->indexLocations[0])));
    
    // Print info
    printf("-------------------------------------------------------\n");
    printf("CREATING NEW DIRECTORY...\n");
    printf("Directory Name: %s\n", tempDir->name);
    printf("Directory Location: %llu\n", tempDir->blockLocation);
    printf("Directory Permissions: %hu\n", tempDir->permissions);
    printf("Directory Creation Date: %u\n", tempDir->dateCreated);
    printf("Parent Directory location: %llu\n", tempDir->parentDirectory);
    printf("Child Directory locations: * No Children Directories *\n");
    printf("-------------------------------------------------------\n\n");
    
    // Write to open block
    LBAwrite(tempDir, 1, freeBlock);
    
    // Update block to used
    setBlockAsUsed(freeBlock, blockSize);
    
    // Update parent node's 'fileIndexLocation' to point to this directory
    addChildDirectoryIndexLocationToParent(parentDirectoryBlockNumber, freeBlock, blockSize);
    
    // Since we created a directory, we must update the directory count
    increaseVCBDirectoryCount(blockSize);
    
    // Cleanup
    free(tempDir);
    
    return freeBlock;
}

void createRootDirectory(uint16_t permissions, uint16_t blockSize) {
    // Create temp directory, which will be written to file system
    struct directoryEntry *tempRootDir = malloc(blockSize);
    
    // Set variables for the root directory
    strcpy(tempRootDir->name, "ROOT");
    strcpy(tempRootDir->fileExtension, DIRECTORY_EXTENSION_NAME);
    tempRootDir->blockLocation = 50;
    tempRootDir->permissions = permissions;
    tempRootDir->dateCreated = (unsigned int)time(NULL);
    tempRootDir->dateModified = (unsigned int)time(NULL);
    tempRootDir->fileSize = 0;
    tempRootDir->parentDirectory = 50;
    
    // Since the root has no files/children directories when created, set these pointers to 0
    memset(tempRootDir->indexLocations, 0x00, (sizeof(tempRootDir->indexLocations)/sizeof(tempRootDir->indexLocations[0])));
    
    // Print info
    printf("-------------------------------------------------------\n");
    printf("CREATING *ROOT* DIRECTORY...\n");
    printf("Directory Name: %s\n", tempRootDir->name);
    printf("Directory Location: %llu\n", tempRootDir->blockLocation);
    printf("Directory Permissions: %hu\n", tempRootDir->permissions);
    printf("Directory Creation Date: %u\n", tempRootDir->dateCreated);
    printf("Child Directory locations: None\n");
    printf("-------------------------------------------------------\n\n");
    
    // Write back the block
    LBAwrite(tempRootDir, 1, 50);
    
    // Since we created a directory, we must update the directory count
    increaseVCBDirectoryCount(blockSize);
    
    //Cleanup
    free(tempRootDir);
}

void increaseVCBDirectoryCount(uint16_t blockSize) {
    // Create a temp volumeControlBlock to gather back information from block 0
    struct volumeControlBlock *vcb = malloc(blockSize);

    // Read from LBA block 0
    LBAread(vcb, 1, 0);
    
    // Update the number of directories counter
    vcb->numberOfDirectories = (vcb->numberOfDirectories + 1);
    
    // Write VCB back to file system
    LBAwrite(vcb, 1, 0);
    
    //Clean up
    free(vcb);
}

void setVCBCurrentDirectory(uint64_t newDirectoryBlock, uint16_t blockSize) {
    // Create a temp volumeControlBlock to gather back information from block 0
    struct volumeControlBlock *vcb = malloc(blockSize);

    // Read from LBA block 0
    LBAread(vcb, 1, 0);
    
    // Update the current dirctory block
    vcb->currentDirectory = newDirectoryBlock;
    
    // Write VCB back to file system
    LBAwrite(vcb, 1, 0);
    
    //Clean up
    free(vcb);
}

void changeDirectory(char* directoryName, uint16_t blockSize) {
    // Print Header
    printf("-------------------------------------------------------\n");
    printf("CHANGING DIRECTORIES...\n");
    
    
    // Get block of the current directory
    uint64_t currentDirectory = getVCBCurrentDirectory(blockSize);
    
    // Get the directory structure of the current directory
    struct directoryEntry *dir = getDirectoryEntryFromBlock(currentDirectory, blockSize);
    
    // If command was 'cd ..', then we are changing directory to the parent
    if (strcmp(directoryName, "..") == 0) {
        // Print Info
        printf("Attempting to change directory to: *PARENT*\n");
        // Save parent block
        uint64_t parentBlock = dir->parentDirectory;
        
        // Get directory struct of the parent
        struct directoryEntry *parentDir = getDirectoryEntryFromBlock(parentBlock, blockSize);
        
        // Set current directory to parent block
        setVCBCurrentDirectory(parentBlock, blockSize);
        
        // Print Info
        printf("Changed Directory To: %s\n", parentDir->name);
        
        // Cleanup
        free (parentDir);
        free (dir);
        printf("-------------------------------------------------------\n\n");
    }
    
    // If command was 'cd NAME', then we have to ensure that NAME is a valid child before changing the directory
    else {
        // Print Info
        printf("Attempting to change directory to: %s\n", directoryName);
        
        // Iterate through all children directories to find a name that matches
        for (int i = 0; dir->indexLocations[i] != 0; i++) {
            // Save block of the child we are going to look at
            uint64_t childBlock = dir->indexLocations[i];
            
            // Pull the child directory
            struct directoryEntry *childDir = getDirectoryEntryFromBlock(childBlock, blockSize);
            
            // If we find the name that matches, then change the current directory to that directorie's block, and break out of loop
            if (strcmp(childDir->name, directoryName) == 0) {
                // Set new directory to the correct child
                setVCBCurrentDirectory(childBlock, blockSize);
                
                printf("Successfully Changed Directory To: %s\n", childDir->name);
                
                // Cleanup
                free (childDir);
                free (dir);
                printf("-------------------------------------------------------\n\n");
                return;
            }
        }
        // If we iterated through all the children and did not find a match, then we cannot change directories
        printf("Directory Not Found!\n");
        printf("-------------------------------------------------------\n\n");
    }
}

void decreaseVCBDirectoryCount(uint16_t blockSize) {
    // Create a temp volumeControlBlock to gather back information from block 0
    struct volumeControlBlock *vcb = malloc(blockSize);

    // Read from LBA block 0
    LBAread(vcb, 1, 0);
    
    // Update the number of directories counter
    // Ensure that we are not going into a negative number
    if ( (vcb->numberOfDirectories - 1) >= 0) {
        vcb->numberOfDirectories = (vcb->numberOfDirectories - 1);
    } else {
        printf("ERROR IN decreaseVCBDirectoryCount(). Attempting to reduce directory count below 0!\n");
    }
    
    // Write VCB back to file system
    LBAwrite(vcb, 1, 0);
    
    //Clean up
    free(vcb);
}

uint64_t getVCBDirectoryCount(uint16_t blockSize) {
    // Create a temp volumeControlBlock to gather back information from block 0
    struct volumeControlBlock *vcb = malloc(blockSize);

    // Read from LBA block 0
    LBAread(vcb, 1, 0);
    
    uint64_t numberOfDirectories = vcb->numberOfDirectories;
    
    //Clean up
    free(vcb);
    
    return numberOfDirectories;
}

uint64_t getVCBRootDirectory(uint16_t blockSize) {
    // Create a temp volumeControlBlock to gather back information from block 0
    struct volumeControlBlock *vcb = malloc(blockSize);

    // Read from LBA block 0
    LBAread(vcb, 1, 0);
    
    uint64_t rootDirectory = vcb->rootDirectory;
    
    //Clean up
    free(vcb);
    
    return rootDirectory;
}

uint64_t getVCBCurrentDirectory(uint16_t blockSize) {
    // Create a temp volumeControlBlock to gather back information from block 0
    struct volumeControlBlock *vcb = malloc(blockSize);

    // Read from LBA block 0
    LBAread(vcb, 1, 0);
    
    uint64_t rootDirectory = vcb->currentDirectory;
    
    //Clean up
    free(vcb);
    
    return rootDirectory;
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
            free(fsi);
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
    
    // Write back to LBA
    LBAwrite(fsi, 49, 1);
    
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

int addChildDirectoryIndexLocationToParent(uint64_t parentDirectoryBlockNumber, uint64_t childDirectoryBlockNumber, int16_t blockSize) {
    // Get directory from LBA block
    struct directoryEntry *tempDir = malloc(blockSize);
    LBAread(tempDir, 1, parentDirectoryBlockNumber);
    
    // Find first free IndexLocation slot
    for(int i = 0; i < (sizeof(tempDir->indexLocations) / sizeof(tempDir->indexLocations[0])); i++) {
        // If the current slot we are checking is empty, update it
        if (tempDir->indexLocations[i] == 0) {
            // Update it
            tempDir->indexLocations[i] = childDirectoryBlockNumber;
            
            // Write directory back into file system
            LBAwrite(tempDir, 1, parentDirectoryBlockNumber);
            
            // Cleanup
            free (tempDir);
            
            // Return success
            return 1;
        }
    }
    
    // If you go through every element, and do not find a slot, then the parent direcory cannot have any more children
    // Cleanup
    free(tempDir);
    
    // Return failure
    return 0;
}

void sampleCreateDirectories(int16_t blockSize) {
    // Create DUMMY DATA for testing (adding, printing, removing, etc...)
    uint64_t rootDirectory = getVCBRootDirectory(blockSize);
    createDirectory("Pictures", rootDirectory, blockSize);
    uint64_t documentEntryLocation = createDirectory("Documents", rootDirectory, blockSize);
    createDirectory("Identifications", documentEntryLocation, blockSize);
    createDirectory("LegalPaperwork", documentEntryLocation, blockSize);
    uint64_t videosLocation = createDirectory("Videos", rootDirectory, blockSize);
    createDirectory("Animations", videosLocation, blockSize);
}

void exitFileSystem(int16_t blockSize) {
    // Print Info
    printf("-------------------------------------------------------\n");
    printf("CLOSING AND EXITING FILE SYSTEM...\n");
    
    // Set the current directory back to the root at close
    setVCBCurrentDirectory(getVCBRootDirectory(blockSize), blockSize);
    
    // Close the partition
    closePartitionSystem();

    // Print success messages
    printf("FILE SYSTEM CLOSED SUCCESSFULLY!\n");
    printf("-------------------------------------------------------\n");
    
    // Exit
    exit (0);
}
