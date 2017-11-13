#ifndef _GLOBALS_H_
#define _GLOBALS_H_

typedef struct {
	char *progName;
	char *arguments[19];
	int pid;
} Command;


extern Command *commands;


#endif /* _GLOBALS_H_ */
