
#include "inodesInDir.h"

short *refs;
int iSize;


void allInodesInDirectories(unsigned char *p, FILE *f) {
  unsigned int mode;
  EOS32_daddr_t addr;
  int i, j;
  unsigned char tempBlockBuffer[BLOCK_SIZE];

  for (i = 0; i < INOPB; i++) {
    mode = get4Bytes(p);
    p += 32;
    id++;
    if (mode != 0 && ((mode & IFMT) == IFDIR)) {

      for (j = 0; j < 6; j++) {
        addr = get4Bytes(p);
        p += 4;
        if (mode != 0 && addr != 0) {
          readBlock(f, addr, tempBlockBuffer);
          inodesDirectoryBlock(tempBlockBuffer);
        }
      }
      addr = get4Bytes(p);
      p += 4;
      if (mode != 0) {

        if (addr != 0) {
          readBlock(f, addr, tempBlockBuffer);
          singleInodesDirectoryBlock(tempBlockBuffer, f);
        }
      }
      addr = get4Bytes(p);
      p += 4;
      if (mode != 0) {

        if (addr != 0) {
          readBlock(f, addr, tempBlockBuffer);
          doubleInodesDirectoryBlock(tempBlockBuffer, f);
        }
      }
    } else {
      p += 32;
    }
  }
}


void inodesDirectoryBlock(unsigned char *p) {
  EOS32_ino_t ino;
  int i;

  for (i = 0; i < DIRPB; i++) {
    ino = get4Bytes(p);
    p += 4;
    if (ino != 0 && ino < iSize) {
      refs[ino] += 1;
    }

    p += DIRSIZ;
  }
}

void singleInodesDirectoryBlock(unsigned char *p, FILE *f) {
  EOS32_daddr_t addr;
  int i;
  unsigned char tempBlockBuffer[BLOCK_SIZE];

  for (i = 0; i < BLOCK_SIZE / sizeof(EOS32_daddr_t); i++) {
    addr = get4Bytes(p);
    p += 4;
    if (addr != 0) {
      readBlock(f, addr, tempBlockBuffer);
      inodesDirectoryBlock(tempBlockBuffer);
    }
  }
}

void doubleInodesDirectoryBlock(unsigned char *p, FILE *f) {
  EOS32_daddr_t addr;
  int i;
  unsigned char tempBlockBuffer[BLOCK_SIZE];

  for (i = 0; i < BLOCK_SIZE / sizeof(EOS32_daddr_t); i++) {
    addr = get4Bytes(p);
    p += 4;
    if (addr != 0) {
      readBlock(f, addr, tempBlockBuffer);
      singleInodesDirectoryBlock(tempBlockBuffer, f);
    }
  }
}
