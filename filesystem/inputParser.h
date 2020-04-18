#ifndef inputParser_h
#define inputParser_h
#include <stdio.h>
#endif /* inputParser_h */

// Takes in user input, and validates if it is a valid input or not
// Returns 1 if inout is valid
// Return 0 if input is invalid
int userInputIsValid (int argc, char *argv[]);

// Takes in a command list, and executes the command
// This function should only be called when the command/arguments have been validated by userInputIsValid()
void executeCommand (int argc, char *argv[], uint64_t blockSize);
