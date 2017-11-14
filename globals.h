#ifndef _GLOBALS_H_
#define _GLOBALS_H_

typedef struct {

	char *progName;
	char *arguments[19];
	int pid;

	int executionError;

	time_t timestampCommand;
	double time;

} Command;

extern Command *commands;

time_t timestamp;


#endif /* _GLOBALS_H_ */
