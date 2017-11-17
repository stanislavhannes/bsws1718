#ifndef _GLOBALS_H_
#define _GLOBALS_H_

typedef struct {

	char *progName;
	char *arguments[19];
	pid_t pid;

	int executionError;
	clock_t time;

} Command;

typedef struct {

	int pid;
	clock_t time;

} TimeList;



extern Command *commands;

clock_t timestamp;


#endif /* _GLOBALS_H_ */
