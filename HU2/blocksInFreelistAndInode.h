
#ifndef _BLOCKSINFREELISTANDINODE_H_
#define _BLOCKSINFREELISTANDINODE_H_

#include "shfs.h"

void superBlock(unsigned char *p);
void datablocks(unsigned char *p, FILE *f);
void freeBlock(unsigned char *p);
void singleIndirectBlock(unsigned char *p);
void doubleIndirectBlock(unsigned char *p, FILE *f);

#endif
