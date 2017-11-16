#ifndef _GLOBALS_H_
#define _GLOBALS_H_

typedef struct {

	char *progName;
	char *arguments[19];
	int pid;

	int executionError;

	clock_t timestampCommand;
	clock_t time;

} Command;

extern Command *commands;

clock_t timestamp;


#endif /* _GLOBALS_H_ */
