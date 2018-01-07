
#include "inodesInDir.h"

short *refs;
int iSize;

/*
  TODO: eventuell die Verzeichnisse rekursiv durchlaufen
*/

void recursive(unsigned char *p, FILE *f) {
  // man bekommt eine Directory-Tabelle durch die Blocknummer
  // in der Tabelle werden Inode-Nummern angegeben

  // wie komme ich von der Inode-Nummer zu den Datenblöcken
  // durch eine Funktion, die einen int (die Inode Nummer bekommt)
  // und solange eine for-Schleife laufen lässt, bis der Inode gefunden wird
  // und danach was tut? und danach wenn es ein dir ist
}

void getInode(int ino) {
  unsigned int mode;
  EOS32_daddr_t addr;
  int i, j;
  unsigned char tempBlockBuffer[BLOCK_SIZE];
  unsigned int currBlock;

  currBlock = 2;
  readBlock(f, currBlock, tempBlockBuffer);

  for (i = 0; i < iSize; i++) {
    if (i == ino) {
      mode = get4Bytes(p);
      p += 32;
      if (mode != 0 && (mode & IFMT) == IFDIR) {

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

        if (addr != 0) {
          readBlock(f, addr, tempBlockBuffer);
          singleInodesDirectoryBlock(tempBlockBuffer, f);
        }

        addr = get4Bytes(p);
        p += 4;

        if (addr != 0) {
          readBlock(f, addr, tempBlockBuffer);
          doubleInodesDirectoryBlock(tempBlockBuffer, f);
        }
      } else {
        p += 32;
      }
    }
    tempBlockBuffer += 64;

    if (i % 63 == 0) {
      currBlock++;
      if (currBlock == 26) { break; }
      readBlock(f, currBlock, tempBlockBuffer);
    }
  }
}

// void allInodesInDirectories(unsigned char *p, FILE *f) {
//   unsigned int mode;
//   EOS32_daddr_t addr;
//   int i, j;
//   unsigned char tempBlockBuffer[BLOCK_SIZE];
//
//   for (i = 0; i < INOPB; i++) {
//     mode = get4Bytes(p);
//     p += 32;
//     if (mode != 0 && (mode & IFMT) == IFDIR) {
//
//       for (j = 0; j < 6; j++) {
//         addr = get4Bytes(p);
//         p += 4;
//         if (mode != 0 && addr != 0) {
//           readBlock(f, addr, tempBlockBuffer);
//           inodesDirectoryBlock(tempBlockBuffer);
//         }
//       }
//       addr = get4Bytes(p);
//       p += 4;
//
//       if (addr != 0) {
//         readBlock(f, addr, tempBlockBuffer);
//         singleInodesDirectoryBlock(tempBlockBuffer, f);
//       }
//
//       addr = get4Bytes(p);
//       p += 4;
//
//       if (addr != 0) {
//         readBlock(f, addr, tempBlockBuffer);
//         doubleInodesDirectoryBlock(tempBlockBuffer, f);
//       }
//     } else {
//       p += 32;
//     }
//   }
// }

void setToRootInode(unsigned char *p, FILE *f) {
  p += 64;
  int root = directoryInodesRecursive(p, f);
}

int directoryInodesRecursive(unsigned char *p, FILE *f) {
  unsigned int mode;
  EOS32_daddr_t addr;
  int j;
  unsigned char tempBlockBuffer[BLOCK_SIZE];

  // es wird vom root-Inode ausgehend jedes Verzeichnis durchlaufen
  // wenn der nächste Inode ein Directory ist, +1 und dieses Directory öffnen
  // ansonsten für einen Daten-Inode +1
  // muss in der main gemacht werden: initial den root mit += 64
  mode = get4Bytes(p);
  p += 32;

  for (j = 0; j < 6; j++) {
    addr = get4Bytes(p);
    p += 4;
    if (addr != 0) {
      readBlock(f, addr, tempBlockBuffer);
      inodesDirectoryBlock(tempBlockBuffer, f);
    }

    addr = get4Bytes(p);
    p += 4;

    if (addr != 0) {
      readBlock(f, addr, tempBlockBuffer);
      singleInodesDirectoryBlock(tempBlockBuffer, f);
    }

    addr = get4Bytes(p);
    p += 4;

    if (addr != 0) {
      readBlock(f, addr, tempBlockBuffer);
      doubleInodesDirectoryBlock(tempBlockBuffer, f);
    }
  }

  return 1;
}


void inodesDirectoryBlock(unsigned char *p, FILE *f) {
  EOS32_ino_t ino;
  unsigned int mode;
  int i;
  unsigned char tempBlockBuffer[BLOCK_SIZE];

  ino = get4Bytes(p);
  refs[ino] += 1;
  p += 64;

  ino = get4Bytes(p);
  refs[ino] += 1;
  p += 64;

  for (i = 2; i < DIRPB; i++) {
    ino = get4Bytes(p);
    refs[ino] += 1;
    // jetzt muss hier dieser inode gelesen werden, indem für diesen Inode
    // gecheckt wird, ob es ein directory ist und falls das der Fall ist,
    // dieses Dir wieder mit inodesDirectoryBlock aufgerufen werden
    //
    // allerdings muss hierfür auf die Datenblöcke des Inodes zugegriffen werden
    //
    // neue DS, die sagt, ob es ein Dir ist?
    // ODER
    // diesen Inode mit directoryInodesRecursive lesen
    // inode 2, wie komme


    // 1 wird gelesen, es sollte aber 27 sein
    readBlock(f, ino, tempBlockBuffer);
    mode = get4Bytes(tempBlockBuffer);

    if (mode != 0 && (mode & IFMT) == IFDIR && ino != 0 && ino < iSize) {
      refs[ino] += 1;
      inodesDirectoryBlock(tempBlockBuffer, f);
    } else {
      if (ino != 0 && ino < iSize) {
        refs[ino] += 1;
      }
    }

    p += 64;
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
      inodesDirectoryBlock(tempBlockBuffer, f);
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
