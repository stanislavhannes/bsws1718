/*
 * shfs.c -- show an EOS32 file system
 */

#include "shfs.h"

EOS32_daddr_t fsize;
int iSize;
unsigned int fsStart;


void readBlock(FILE *disk, EOS32_daddr_t blockNum, unsigned char *blockBuffer) {
  fseek(disk, fsStart * SECTOR_SIZE + blockNum * BLOCK_SIZE, SEEK_SET);
  if (fread(blockBuffer, BLOCK_SIZE, 1, disk) != 1) {
    error("cannot read block %lu (0x%lX)", 99, blockNum, blockNum);
  }
}


unsigned int get4Bytes(unsigned char *addr) {
  return (unsigned int) addr[0] << 24 |
         (unsigned int) addr[1] << 16 |
         (unsigned int) addr[2] <<  8 |
         (unsigned int) addr[3] <<  0;
}



void filesystemSize(unsigned char *p) {
  p += 4;
  fsize = get4Bytes(p);
  iSize = 24*64;
}


void error(char *fmt, int exitcode, ...) {
  va_list ap;

  va_start(ap, exitcode);
  printf("Error: ");
  vprintf(fmt, ap);
  printf("\n");
  va_end(ap);
  exit(exitcode);
}
