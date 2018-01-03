
#include "checkInodesAndBlocks.h"

EOS32_daddr_t fsize;
Block *blocks;
short *refs;

/*

geht die Datenstruktur blocks durch und überprüft auf Fehler

*/
void blockCheck() {
  for (int i = 26; i < fsize; i++) {
    printf("%d - %d - %d \n", i, blocks[i].freeList, blocks[i].dataList);
    if ((blocks[i].dataList == 1 && blocks[i].freeList == 0) ||
        (blocks[i].dataList == 0 && blocks[i].freeList == 1)) {
      continue;
    } else {

      if (blocks[i].dataList == 0 && blocks[i].freeList == 0) {
        // TODO: Fehler A
      }

      if (blocks[i].dataList > 0 && blocks[i].freeList > 0) {
        // TODO: Fehler B
      }

      if (blocks[i].freeList > 1) {
        // TODO: Fehler C
      }

      if (blocks[i].dataList > 1) {
        // TODO: Fehler D
      }
    }
  }
}


/*

überprüft die Datensturktur refs auf Fehler

*/
void checkLinkcount(unsigned char *p) {
  unsigned int mode;
  unsigned int nlink;
  int i;

  for (i = 0; i < INOPB; i++) {
    id++;
    mode = get4Bytes(p);
    p += 4;
    nlink = get4Bytes(p);
    printf("%d - %d - %d\n", id, nlink, refs[id]);
    if (mode != 0) {
      if (nlink != refs[id]) {
        // TODO: Error H
      }

      if (nlink == 0 && refs[id] != 0) {
        // TODO: Error F
      }
    } else {
      if (refs[id] != 0) {
        // TODO: Error J
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
      // TODO: Error G
    }
    p += 60;
  }
}

/*

datasize, angegeben im Inode, wird überprüft. Alle Datenblöcke werden gezählt
und über die Anzahl die Dateigröße berechnet. Single und Double indirect Block
wird analog zu inodesInDir.c durchgegangen

TODO: block special und character special Inodes eventuell auslassen

*/
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
      // TODO: Error E
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
