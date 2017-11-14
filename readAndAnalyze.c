#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

#include "readAndAnalyze.h"
#include "makeProcess.h"
#include "globals.h"

char input[500];
Command *commands;

pid_t wpid;
int status;

int main() {

	commands = calloc(10, sizeof(Command));

	while(1) {

		printf("> ");

		if(fgets(input, sizeof(input), stdin) == NULL) {exit(0);}

		// remove '\n' from input
		int n = 0;
		while (input[n] != '\0') {
			if (input[n] == '\n') {input[n] = '\0';}
			n++;
		}

		// store progname and the arguments inside commands
		char *tempCommands[10];
		char delimiter[] = ";";
		int i = 0;


		tempCommands[i] = strtok(input, delimiter);

		while(tempCommands[i] != NULL && i < 10) {
			i++;
			tempCommands[i] = strtok(NULL, delimiter);
		}

		for (int j = 0; j < i; j++) {
			char delimiterTwo[] = " ";
			char *temp;
			int k = 0;

			temp = strtok(tempCommands[j], delimiterTwo);
			commands[j].progName = temp;
			commands[j].arguments[k++] = temp;

			while(temp != NULL && k < 20) {
				temp = strtok(NULL, delimiterTwo);
				commands[j].arguments[k] = temp;
				k++;
			}
		}

		// log the prog and the arguments to the console
		test();
		createProcess(commands);

		// set commands and input to NULL
		memset(input, 0, sizeof (input));
		memset(commands, 0, sizeof(Command) * 10);
	}

	return 0;
}

// arguments are labeld with a ':' just for the console
void test() {

	for (int i=0; i < 10; i++) {
		if (commands[i].progName != NULL) {
			printf("%s\n", commands[i].progName);

		}

		for (int j=0; j < 19; j++) {
			if (commands[i].arguments[j] != NULL) {
				printf(":%s\n", commands[i].arguments[j]);
			}
		}
	}
}
