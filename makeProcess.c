#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <stdio.h>
#include <string.h>
#include <sys/times.h>
#include <time.h>
#include <sys/wait.h>

#include "makeProcess.h"

//TODO: allgemeine Fehlerbehandlung
void createProcess(Command *commands) {

	pid_t wpid;
	int status, n;
	struct tms cutime;
	clock_t cutimeHelpVar;
	int numberofCommands = getNumberOfCommands(commands);

	doFork(numberofCommands, commands);

	// wait for childs and calculate the child user time
	n = numberofCommands;
	cutimeHelpVar = 0;

	times(&cutime);

	while (n > 0) {
	  wpid = wait(&status); // TODO: status in commands speichern

		times(&cutime);

		for (int i = 0; i < numberofCommands; i++) {
			if (commands[i].pid == wpid && status == 0) {
				commands[i].time = cutime.tms_cutime - cutimeHelpVar;
				break;
			} //else if (status == 0) { //TODO: allgemeine Fehlerbehandlung
				//commands[i].time = -1;
			//}
		}

		cutimeHelpVar = cutime.tms_cutime;
		n--;
	}

	cutimeHelpVar = 0;

	printCommands(commands, numberofCommands);

}

void doFork(int numberofCommands, Command *commands) {
	pid_t child_pid;
	int a;

	for (int i=0; i < numberofCommands; i++) {

			switch (child_pid = fork()) {

				case -1:

					perror("fork error");
					exit(1);

				case 0:
					a = 0;

					while (a < 100000000) {a++;}

					printf ("Kindprozess: %d (PID: %d)\n", i, getpid());

					execvp(commands[i].progName, commands[i].arguments);
				// status_execvp should be -1 if execvp get an error
				// execvp only returns when an error occurs
				// We only reach this point as a result of a failure from execvp
				// exit(status_execvp);
					exit(1);

				default:
					commands[i].pid = child_pid;
		}
	}
}

// logs the user time per command to the console
void printCommands(Command *commands, int numberofCommands) {
	for (int i=0; i < numberofCommands; i++) {

			if (commands[i].executionError == 0) {
				printf("%s: user time = %lu\n", commands[i].progName, commands[i].time);
			}

			if (commands[i].executionError == -1) {
				printf("%s: [execution error]\n", commands[i].progName);
			}

	}
}

int getNumberOfCommands(Command *commands) {
	int n = 0;
	for (int i=0; i < 10; i++) {
		if (commands[i].progName != NULL) {
			n++;
		}
	}
	return n;
}
