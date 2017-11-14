#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <stdio.h>
#include <string.h>
#include <sys/times.h>
#include <time.h>

#include "makeProcess.h"


void createProcess(Command *commands) {

	pid_t child_pid, wpid;
	int status;
	static struct tms st_cpu;

	for (int i=0; i < 10; i++) {

		if (commands[i].progName != NULL) {

			child_pid = fork();

			switch (child_pid) {

				case -1:

				perror("fork error");
				exit(1);

				case 0:

				printf ("Kindprozess: %d (PID: %d)\n", i, getpid());

				commands[i].pid = getpid();
				// hier timestamp erlaubt? im Praktikum nachfragen
				commands[i].timestampCommand = times(&st_cpu);
				printf("%jd\n", (intmax_t)commands[i].timestampCommand);
				commands[i].executionError = 0;

				int status_execvp = execvp(commands[i].progName, commands[i].arguments);
				if (status_execvp == -1) { commands[i].executionError = -1; }

				// status_execvp should be -1 if execvp get an error
				// execvp only returns when an error occurs
				// We only reach this point as a result of a failure from execvp
				// exit(status_execvp);
			}
		}
	}

	while ((wpid = wait(&status)) > 0);

	timestamp = times(&st_cpu);
	printf("%jd\n", (intmax_t)timestamp);

	calcChildUserTime(commands);
	printCommands(commands);

}

// calculates the user time per child
void calcChildUserTime(Command *commands) {
	for (int i=0; i < 10; i++) {
		if (commands[i].progName != NULL) {

			// calculation doesn't work ... just the timestamps set to commands[i].time
			// sind die Zahlen eventuell zu groß? <- hier weiter
			// https://cboard.cprogramming.com/c-programming/93528-adding-large-number-digit-digit.html
			commands[i].time = timestamp - commands[i].timestampCommand;
		}
	}
}

// logs the user time per command to the console
void printCommands(Command *commands) {
	for (int i=0; i < 10; i++) {
		if (commands[i].progName != NULL) {

			if (commands[i].executionError == 0) {
				printf("%s: user time = %f\n", commands[i].progName, commands[i].time);
			}

			if (commands[i].executionError == -1) {
				printf("%s: [execution error]\n", commands[i].progName);
			}

		}
	}
}
