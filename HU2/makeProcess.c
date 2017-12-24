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

	doFork(commands);


	// wait for childs and calculate the child user time
	n = numberofCommands;
	cutimeHelpVar = 0;

	times(&cutime);

	while (n > 0) {
	  wpid = wait(&status); // TODO: status in commands speichern

		times(&cutime);

		for (int i=0; i < 10; i++) {
			if (commands[i].progName != NULL) {
				if (commands[i].pid == wpid) {
					if (status == 0) {
						commands[i].time = cutime.tms_cutime - cutimeHelpVar;
						commands[i].status = 0;
						break;
					} else {
						
						if (WIFEXITED(status)) { // WIFSIGNALED
							commands[i].status = -1;
							break;
						} if (WIFSIGNALED(status)) {
							commands[i].status = -1;
							break;
						} else {
							commands[i].status = -1;
							break;
						}
					}
				}
			}
		}

		cutimeHelpVar = cutime.tms_cutime;
		n--;
	}

	cutimeHelpVar = 0;

	printCommands(commands);

}

void doFork(Command *commands) {
	pid_t child_pid;

	for (int i=0; i < 10; i++) {
		if (commands[i].progName != NULL) {

			switch (child_pid = fork()) {

				case -1:

					perror("fork error");
					exit(1);

				case 0:

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
}

// logs the user time per command to the console
void printCommands(Command *commands) {
	int sum = 0;
	int temp = 0;

	for (int i=0; i < 10; i++) {
		if (commands[i].progName != NULL) {
			temp++;

			if (commands[i].status == 0) {
				//printf("%s: user time = %lu\n", commands[i].progName, commands[i].time);
				printf("%s: user time = %i\n", commands[i].progName, commands[i].time);
				sum += commands[i].time;
			}

			if (commands[i].status != 0) {
				printf("%s: [execution error]\n", commands[i].progName);
			}
		}

	}
	if (temp > 0) { printf("sum of user times = %d\n", sum); }
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
