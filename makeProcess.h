#include "globals.h"

void createProcess(Command *commands);
//void do_execvp(char *progName, char *arguments[]);
void calcChildUserTime(Command *commands, int numberofCommands);
void printCommands(Command *commands, int numberofCommands);
int getNumberOfCommands(Command *commands);
