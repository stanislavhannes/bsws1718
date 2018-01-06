#ifndef _CHECKIANDB_H_
#define _CHECKIANDB_H_

#include "shfs.h"

void blockCheck();
void checkLinkcount(unsigned char *p);
void inodeIsFree(unsigned char *p);
void datasize(unsigned char *p, FILE *f);
int singleIndirectBlockdata(unsigned char *p);
int doubleIndirectBlockdata(unsigned char *p, FILE *f);
void checkInodeMode(unsigned char *p);
void checkRootInode(unsigned char *p);

#endif
