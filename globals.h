#ifndef _GLOBALS_H_
#define _GLOBALS_H_

typedef struct {

	char *progName;
	char *arguments[21];
	pid_t pid;

	int status;
	//clock_t time;
	clockid_t time;

} Command;

extern Command *commands;


#endif /* _GLOBALS_H_ */
