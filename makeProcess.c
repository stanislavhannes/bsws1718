#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <stdio.h>
#include <string.h>
#include <sys/times.h>

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
				execvp(commands[i].progName, commands[i].arguments);
				commands[i].timestamp = times(&st_cpu);

				exit (0);
			}
		}
	}

	// Vaterprozess wartet hier auf das Kind
	// wait(): on success, returns the process ID of the terminated child;
  // on error???, -1 is returned.

	// ^^ on error oder auch success, morgen

	while ((wpid = wait(&status)) > 0) {
		printf("test, wpid: %d\n", wpid);
		// eventuell (noch zu testen...):
		//
		// solange sich der Vater-Prozess im wait befindet wird die PID vom Kindprozess
		// in wpid gespeichert - und solange muss hier die Zeit mit wpid im Objekt
		// verändert werden
		//
		// morgen
		//
	}
	
	printf("%d\n", wpid);
}
