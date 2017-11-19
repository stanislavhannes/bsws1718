#ifndef _GLOBALS_H_
#define _GLOBALS_H_

typedef struct {

	char *progName;
	char *arguments[19];
	pid_t pid;

	int status;
	clock_t time;

} Command;

extern pid_t parentPid;

extern Command *commands;


#endif /* _GLOBALS_H_ */
