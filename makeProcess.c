#include <sys/types.h>

void createProcess() {


  pid_t *pids;

  pids = malloc(sizeof(pid_t) * 1);

    int i = 0;
		switch (pids[i] = fork()) {
        case -1:
        /* Wird ein negativer Wert zurückgegeben, ist ein Fehler aufgetreten */
            perror("fork error");
            exit(1);

        case 0:
        /* Kindprozess
       * wenn fork() eine 0 zurückgibt, befinden wir uns im Kindprozess
       */

            /*setsignals*/

            /* notfalls raus 
            if (umlenkungen(aktuellesKommando))
                exit(1); */

            /* Fuer den ersten Pipeline-Teilnehmer nur Ausgabe umlenken 
            dup2(pipefd[1], 1);
            closeAll(pipefd, pipefdcount);

            do_execvp(0, aktuellesKommando->u.einfach.worte);*/

        default: 
        break;
        /* Elternprozess
       * Gibt fork() einen Wert größer 0 zurück, befinden wir uns im Elternprozess
       * in pid steht die ID des Kindprozesses
       * getpid() gibt die eigene PID zurück
       */
    	}
} 

void do_execvp(int argc, char **args) {
    execvp(*args, args);
    perror("exec-Fehler");
    /*fprintf(stderr, "bei Aufruf von \"%s\"\n", *args);*/
    /* proclist = removeFromProcessList(proclist, getProcess(proclist, getpid())); */

    exit(1);
}
