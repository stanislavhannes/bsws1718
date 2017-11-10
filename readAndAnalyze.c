#include<stdio.h>
#include<stdlib.h>
#include <string.h>

#include "readAndAnalyze.h"
#include "makeProcess.h"


char input[500];

typedef struct {
	char *progName;
	char *arguments[19];
} Command;

Command commands[10];

int main() {
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

			while(temp != NULL && k < 19) {
				temp = strtok(NULL, delimiterTwo);
				commands[j].arguments[k] = temp;
				k++;
			}
		}

		// log the prog and the arguments to the console
		test();

		// set commands and input to NULL
		memset(input, 0, sizeof input);
		memset(commands, 0, sizeof commands);
	}

	return 0;
}

// arguments are labeld with a ':' just for the console
void test() {
	for (int i=0; i < 10; i++) {
		if (commands[i].progName != NULL) {printf("%s\n", commands[i].progName);}

		for (int j=0; j < 19; j++) {
			if (commands[i].arguments[j] != NULL) {printf(":%s\n", commands[i].arguments[j]);}
		}
	}
}

/*
		Beachte!

		2) Die Eingabezeile besteht aus beliebig vielen "Commanden" (maximal 10,
		moeglicherweise auch keinem), die jeweils durch ein Semikolon voneinander getrennt sind.

		3) Ein Command besteht aus "Worten" (maximal 20, moeglicherweise auch keinem):
		ein Programmname, gefolgt von bis zu 19 Argumenten, die jeweils durch ein oder
		mehrere Leerzeichen voneinander getrennt sind.
*/
