#include "globals.h"

void createProcess(Command *commands);
void doFork(int numberofCommands, Command *commands);
void printCommands(Command *commands, int numberofCommands);
int getNumberOfCommands(Command *commands);
void handlerSigint (int sig);
