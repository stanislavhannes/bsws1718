#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

#include "makeProcess.h"
#include "globals.h"

// void test(Command *commands);

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

			// end with NULL
			commands[j].arguments[k] = NULL;
		}

		// test(commands);

		createProcess(commands);

		// set commands and input to NULL
		commands = calloc(10, sizeof(Command));
		strcpy(input, "");
	}

	return 0;
}

/*Wenn ein Befehl gaenzlich leer ist (also nicht einmal einen Programmnamen
enthaelt), wird er ignoriert. Wenn eine Eingabezeile gaenzlich leer ist
(also keinen einzigen Befehl enthaelt), wird sie ignoriert. */

/*void test(Command *commands) {
	for (int i = 0; i < 10; i++) {
		printf("i: %d\n", i);
		printf("name: %s\n", commands[i].progName);
		for (int j = 0; j < 21; j++) {
			printf(":%s\n", commands[i].arguments[j]);
		}
	}
}*/
