
#include "inodesInDir.h"

// 2 mal die Variable refs? in chkfs.c ist sie bereits angelegt
short *refs;

/*

allInodesInDirectories() findet alle Inodes, die ein Directory sind. Der mode
muss != 0 sein und mode & IFMT == IFDIR. IFDIR sagt, dass es ein Directory ist.

Wenn der Inode ein Directory ist, wird für jeden dataBlock die Funktion
inodesDirectoryBlock() aufgerufen, um quasi die InodeDatei zu lesen und deren
Inhalt in die Datenstruktur refs zu speichern. Wenn es bis zu einem singleIndirectBlock
geht, wird die Funktion inodesDirectoryBlock() über die Funktion
singleInodesDirectoryBlock aufgerufen, damit die "Weiche" richtig gestellt werden
kann. Am besten einmal aufmalen... Das gleiche gilt für den doubleIndirectBlock,
allerdings mit einer Verzweigung mehr. Es ist unwahrscheinlich, dass ein Ver-
zeichnis so groß wird, aber für den Fall der Fälle ist es implementiert

=> if (mode != 0 && ((mode & IFMT) == IFDIR)) { ... } else { p += 32; }

else: der Zeiger p wird um 32 erhöht, wenn der Inode kein Directory ist, um den
nächsten Inode zu checken

*/
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

/*

ino ist die ID der Datei (= die ID des Inodes der Datei)
refs[ino] += 1 => der Inode ist n mal in einem Directory

*/
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
