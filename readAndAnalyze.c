#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

#include "makeProcess.h"
#include "globals.h"

char input[500];
Command *commands;

//TODO: allgemeine Fehlerbehandlung
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

		// remove quotation marks from input
		n = 0;
		while (input[n] != '\0') {
			if (input[n] == '\"') {input[n] = ' ';}
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
			commands[j].arguments[k] = temp;

			k++;

			while(temp != NULL && k < 20) {
				temp = strtok(NULL, delimiterTwo);
				commands[j].arguments[k] = temp;
				k++;
			}
			if (k == 20) {
				commands[j].arguments[20] = NULL;
			} else {
				commands[j].arguments[k] = NULL;
			}
		}

		createProcess(commands);

		// set commands and input to NULL
		memset(input, 0, sizeof (input));
		memset(commands, 0, sizeof(Command) * 10);
	}

	return 0;
}
