#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <stdio.h>
#include <string.h>
#include "makeProcess.h"

void createProcess(Command *commands) {

	pid_t pid;


	for (int i=0; i < 10; i++) {

		if (commands[i].progName != NULL) {

			pid = fork();

			switch (pid) {
				case -1:
		        /* Wird ein negativer Wert zurückgegeben, ist ein Fehler aufgetreten */
				perror("fork error");
				exit(1);

				case 0:

				/* Kindprozess
				* wenn fork() eine 0 zurückgibt, befinden wir uns im Kindprozess
				*/
				
				
				printf("%s\n", commands[i].progName);
				printf ("Kindprozess: %d (PID: %d)\n", i, getpid());

				commands[i].pid = getpid();
				execvp(commands[i].progName, commands[i].arguments);

			

				exit (0);

		            /*setsignals*/

		            /* notfalls raus 
		            if (umlenkungen(aktuellesKommando))
		                exit(1); */

		            /* Fuer den ersten Pipeline-Teilnehmer nur Ausgabe umlenken 
		            dup2(pipefd[1], 1);
		            closeAll(pipefd, pipefdcount);

		            do_execvp(0, aktuellesKommando->u.einfach.worte);*/

				default: 
		        /* Elternprozess
		       * Gibt fork() einen Wert größer 0 zurück, befinden wir uns im Elternprozess
		       * in pid steht die ID des Kindprozesses
		       * getpid() gibt die eigene PID zurück
		       */
				for (i=0; i < 10; i++) {
					printf ("Elternprozess: %d (PID: %d)\n", i, getpid());
					sleep (1);
				}
			}
		}
	}
} 

void do_execvp(int argc, char **args) {
	execvp(*args, args);
	perror("exec-Fehler");
		    /*fprintf(stderr, "bei Aufruf von \"%s\"\n", *args);*/
		    /* proclist = removeFromProcessList(proclist, getProcess(proclist, getpid())); */

	exit(1);
}
