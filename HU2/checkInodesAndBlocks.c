
#include "checkInodesAndBlocks.h"

EOS32_daddr_t fsize;
Block *blocks;
short *refs;
int id;


void blockCheck() {
  for (int i = 26; i < fsize; i++) {
    if ((blocks[i].dataList == 1 && blocks[i].freeList == 0) ||
        (blocks[i].dataList == 0 && blocks[i].freeList == 1)) {
      continue;
    } else {

      if (blocks[i].dataList == 0 && blocks[i].freeList == 0) {
        error("A block is neither in a file nor on the free list", 10);
      }

      if (blocks[i].dataList > 0 && blocks[i].freeList > 0) {
        error("A block is both, in a file and on the free list", 11);
      }

      if (blocks[i].freeList > 1) {
        error("A block is more then once on the free list", 12);
      }

      if (blocks[i].dataList > 1) {
        error("A block is more then once in a file", 13);
      }
    }
  }
}


void checkLinkcount(unsigned char *p) {
  unsigned int mode;
  unsigned int nlink;
  int i;

  for (i = 0; i < INOPB; i++) {
    id++;
    mode = get4Bytes(p);
    p += 4;
    nlink = get4Bytes(p);
    //printf("%d - %d - %d\n", id, nlink, refs[id]);
    if (mode != 0) {
      if (nlink != refs[id]) {
        error("An inode with linkcount n! = 0 does not appear in exactly n directories", 17);
      }

      if (nlink == 0 && refs[id] != 0) {
        error("An inode with linkcount 0 appears in a directory", 15);
      }
    } else {
      if (refs[id] != 0) {
        error("An inode appears in a directory but is free", 19);
      }
    }

    p += 60;
  }
}

void inodeIsFree(unsigned char *p) {
  int i;
  unsigned int mode;
  unsigned int nlink;

  for (i = 0; i < INOPB; i++) {

    mode = get4Bytes(p);
    p += 4;
    nlink = get4Bytes(p);
    if (nlink == 0 && mode != 0) {
      error("An inode with linkcount 0 is not free", 16);
    }
    p += 60;
  }
}

void inodeIsFreeWithoutRoot(unsigned char *p) {
  int i;
  unsigned int mode;
  unsigned int nlink;

  for (i = 0; i < INOPB; i++) {
    if (i == 0) {
      p += 64;
      continue;
    }
    mode = get4Bytes(p);
    p += 4;
    nlink = get4Bytes(p);
    if (nlink == 0 && mode != 0) {
      error("An inode with linkcount 0 is not free", 16);
    }
    p += 60;
  }
}

void datasize(unsigned char *p, FILE *f) {
  unsigned int mode;
  EOS32_daddr_t addr;
  EOS32_off_t size;
  EOS32_off_t calcSize;
  int i, j;
  int number;
  unsigned char blockBufferDatasize[BLOCK_SIZE];

  for (i = 0; i < INOPB; i++) {
    number = 0;

    mode = get4Bytes(p);

    if (mode != 0) {

      if ((mode & IFMT) == IFCHR || (mode & IFMT) == IFBLK) {
        p += 64;
        continue;
      }

      p += 28;
      size = get4Bytes(p);
      p += 4;

      for (j = 0; j < 6; j++) {
        addr = get4Bytes(p);
        p += 4;
        if (mode != 0 && addr != 0) {
          number++;
        }
      }
      addr = get4Bytes(p);
      p += 4;
      if (mode != 0) {

        if (addr != 0) {
          readBlock(f, addr, blockBufferDatasize);
          number += singleIndirectBlockdata(blockBufferDatasize);
        }
      }
      addr = get4Bytes(p);
      p += 4;
      if (mode != 0) {

        if (addr != 0) {
          readBlock(f, addr, blockBufferDatasize);
          number += doubleIndirectBlockdata(blockBufferDatasize, f);
        }
      }

      calcSize = number * BLOCK_SIZE;
      if (size > calcSize || size <= (calcSize-BLOCK_SIZE)) {
        error("The size of a file is not consistent with the blocks noted in the inode", 14);
      }
    } else {
      p += 64;
    }
  }
}

int singleIndirectBlockdata(unsigned char *p) {
  EOS32_daddr_t addr;
  int i;
  int number = 0;

  for (i = 0; i < BLOCK_SIZE / sizeof(EOS32_daddr_t); i++) {
    addr = get4Bytes(p);
    p += 4;
    if (addr != 0) {
      number++;
    }
  }

  return number;
}

int doubleIndirectBlockdata(unsigned char *p, FILE *f) {
  EOS32_daddr_t addr;
  int i;
  unsigned char blockBufferDatasize[BLOCK_SIZE];
  int number = 0;

  for (i = 0; i < BLOCK_SIZE / sizeof(EOS32_daddr_t); i++) {
    addr = get4Bytes(p);
    p += 4;

    if (addr != 0) {
      readBlock(f, addr, blockBufferDatasize);
      number += singleIndirectBlockdata(blockBufferDatasize);
    }
  }
  return number;
}

void checkInodeMode(unsigned char *p) {
  unsigned int mode;
  int i;

  for (i = 0; i < INOPB; i++) {
    mode = get4Bytes(p);
    if (mode != 0) {
      if ((mode & IFMT) == IFREG) {
        p += 64;
        continue;
      } else
      if ((mode & IFMT) == IFDIR) {
        p += 64;
        continue;
      } else
      if ((mode & IFMT) == IFCHR) {
        p += 64;
        continue;
      } else
      if ((mode & IFMT) == IFBLK) {
        p += 64;
        continue;
      } else {
        error("The inode has a type field with an illegal value!", 18);
      }
    }
  }
}

void checkRootInode(unsigned char *p) {
  unsigned int mode;

  p += 64;
  mode = get4Bytes(p);
  if (!((mode & IFMT) == IFDIR)) {
    error("The root inode is not a directory!", 20);
  }
}
