#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <signal.h>

#include "makeProcess.h"
#include "globals.h"
#include "readAndAnalyze.h"

char input[500];
Command *commands;
int end = 1;

//TODO: allgemeine Fehlerbehandlung
int main() {

	signal(SIGINT, handlerSigint);
	commands = calloc(10, sizeof(Command));

	while(end) {

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
		memset(commands, 0, sizeof(Command) * 10);
		strcpy(input, "");
	}

	return 0;
}

void handlerSigint (int sig){

   	if(sig != SIGINT)

      return;

  	else {
   		end = 0;
   }
}
