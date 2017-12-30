
#ifndef _INODESINDIR_H_
#define _INODESINDIR_H_

#include "shfs.h"

void allInodesInDirectories(unsigned char *p, FILE *f);
void inodesDirectoryBlock(unsigned char *p);
void singleInodesDirectoryBlock(unsigned char *p, FILE *f);
void doubleInodesDirectoryBlock(unsigned char *p, FILE *f);

#endif

