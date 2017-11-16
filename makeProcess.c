#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <stdio.h>
#include <string.h>
#include <sys/times.h>
#include <time.h>
#include <sys/wait.h>

#include "makeProcess.h"


void createProcess(Command *commands) {

	pid_t child_pid, wpid;
	int status;
	struct tms cutime1;
	//cutime2;
	int numberofCommands = getNumberOfCommands(commands);
	// Anzahl der commands in n speichern durch eine neue Funktion

	for (int i=0; i < numberofCommands; i++) {

			child_pid = fork();

			switch (child_pid) {

				case -1:

					perror("fork error");
					exit(1);

				case 0:

					printf ("Kindprozess: %d (PID: %d)\n", i, getpid());

					commands[i].pid = getpid();
					commands[i].executionError = 0;

					execvp(commands[i].progName, commands[i].arguments);

				// status_execvp should be -1 if execvp get an error
				// execvp only returns when an error occurs
				// We only reach this point as a result of a failure from execvp
				// exit(status_execvp);
		}
	}

	times(&cutime1);

	printf("Test start_tms.tms_utime = %f\n\n",  (double)cutime1.tms_utime);
  printf("Test start_tms.tms_cutime = %f\n\n", (double)cutime1.tms_cutime);
  printf("Test start_tms.tms_stime = %f\n\n",  (double)cutime1.tms_stime);
  printf("Test start_tms.tms_cstime = %f\n\n",  (double)cutime1.tms_cstime);

	int n = numberofCommands;

	while (n > 0) {
	  wpid = wait(&status);

	  printf("Test start_tms.tms_cutime = %f\n\n", (double)cutime1.tms_cutime);
	  printf("Child with PID %ld exited with status 0x%x.\n", (long)wpid, status);
	  n--;
	}

	// printf("%jd\n", (intmax_t)timestamp);

	//calcChildUserTime(commands, numberofCommands);
	//printCommands(commands, numberofCommands);

}

// TODO: calculates the user time per child
void calcChildUserTime(Command *commands, int numberofCommands) {
	for (int i=0; i < numberofCommands; i++) {
			commands[i].time = timestamp - commands[i].timestampCommand;
	}
}

// TODO: logs the user time per command to the console
void printCommands(Command *commands, int numberofCommands) {
	for (int i=0; i < numberofCommands; i++) {

			if (commands[i].executionError == 0) {
				//printf("%s: user time = %f\n", commands[i].progName, commands[i].time);
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
