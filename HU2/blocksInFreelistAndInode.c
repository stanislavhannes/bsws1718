
#include "blocksInFreelistAndInode.h"

EOS32_daddr_t linkBlock;
Block *blocks;

void superBlock(unsigned char *p) {

  unsigned int nfree;
  EOS32_daddr_t free;
  int i;

  p += 24;

  // for (i = 0; i < NICINOD; i++) { p += 4; }
  p = p + (NICINOD * 4);

  nfree = get4Bytes(p);
  p += 4;

  for (i = 0; i < NICFREE; i++) {
    free = get4Bytes(p);
    p += 4;
    if (i == 0) {
      linkBlock = free;
      if (free != 0) { blocks[free].freeList += 1; }
    } else {
        if (i < nfree) {
          blocks[free].freeList += 1;
        }
      }
  }
}

void datablocks(unsigned char *p, FILE *f) {
  unsigned int mode;
  EOS32_daddr_t addr;
  int i, j;
  unsigned char tempBlockBuffer[BLOCK_SIZE];

  for (i = 0; i < INOPB; i++) {

    mode = get4Bytes(p);
    p += 32;

    if ((mode & IFMT) == IFCHR || (mode & IFMT) == IFBLK) {
      p += 32;
      continue;
    }

    for (j = 0; j < 6; j++) {
      addr = get4Bytes(p);
      p += 4;
      if (mode != 0 && addr != 0) {
        blocks[addr].dataList += 1;
      }
    }
    addr = get4Bytes(p);
    p += 4;
    if (mode != 0) {

      if (addr != 0) {
        blocks[addr].dataList += 1;
        readBlock(f, addr, tempBlockBuffer);
        singleIndirectBlock(tempBlockBuffer);
      }
    }
    addr = get4Bytes(p);
    p += 4;
    if (mode != 0) {

      if (addr != 0) {
        blocks[addr].dataList += 1;
        readBlock(f, addr, tempBlockBuffer);
        doubleIndirectBlock(tempBlockBuffer, f);
      }
    }
  }
}

void freeBlock(unsigned char *p) {
  unsigned int nfree;
  EOS32_daddr_t addr;
  int i;

  nfree = get4Bytes(p);
  p += 4;
  for (i = 0; i < NICFREE; i++) {
    addr = get4Bytes(p);
    p += 4;
    if (i == 0) {
      linkBlock = addr;
      if (addr != 0) { blocks[addr].freeList += 1; }
    } else {
        if (i < nfree) {
          blocks[addr].freeList += 1;
        }
      }
  }
}

void singleIndirectBlock(unsigned char *p) {
  EOS32_daddr_t addr;
  int i;

  for (i = 0; i < BLOCK_SIZE / sizeof(EOS32_daddr_t); i++) {
    addr = get4Bytes(p);
    p += 4;
    if (addr != 0) {
      blocks[addr].dataList += 1;
    }
  }
}

void doubleIndirectBlock(unsigned char *p, FILE *f) {
  EOS32_daddr_t addr;
  int i;
  unsigned char tempBlockBuffer[BLOCK_SIZE];

  for (i = 0; i < BLOCK_SIZE / sizeof(EOS32_daddr_t); i++) {
    addr = get4Bytes(p);
    p += 4;
    if (addr != 0) {
      blocks[addr].dataList += 1;
      readBlock(f, addr, tempBlockBuffer);
      singleIndirectBlock(tempBlockBuffer);
    }
  }
}

