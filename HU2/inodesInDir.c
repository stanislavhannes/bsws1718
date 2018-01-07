
#include "inodesInDir.h"

Refs *refs;
int iSize;

int currBlock = 1;

/*
  TODO: eventuell die Verzeichnisse rekursiv durchlaufen
*/

// Methode um den Block um eins zu erhöhen und einen Zeiger auf tempBlockBuffer
// weitergeben


void recursiveDirectory(int ino, FILE *f) {
  unsigned int mode;
  EOS32_daddr_t addr;
  int i, j;
  int currBlock;
  unsigned char tempBlockBuffer[BLOCK_SIZE];
  unsigned char *p;

  currBlock = 2;
  readBlock(f, currBlock, tempBlockBuffer);
  p = tempBlockBuffer;

  for (i = 0; i < iSize; i++) {
    if (i == ino) {
      mode = get4Bytes(p);
      p += 32;
      if (mode != 0 && (mode & IFMT) == IFDIR) {
        refs[ino].flag = 1;
        refs[ino].linkcount += 1;
        for (j = 0; j < 6; j++) {
          addr = get4Bytes(p);
          p += 4;
          if (addr != 0) {
            readBlock(f, addr, tempBlockBuffer);
            inodesDirectoryBlock(tempBlockBuffer, f);
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
        refs[ino].linkcount += 1;
        break;
      }

      break;
    }

    if (i != 0 && i % 63 == 0) {
      currBlock++;
      readBlock(f, currBlock, tempBlockBuffer);
      p = tempBlockBuffer;
      continue;
    }

    p += 64;
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

// start mit recursiveDirectory(1)?
// void setToRootInode(unsigned char *p, FILE *f) {
//   p += 64;
//   int root = directoryInodesRecursive(p, f);
// }
//
// int directoryInodesRecursive(unsigned char *p, FILE *f) {
//   unsigned int mode;
//   EOS32_daddr_t addr;
//   int j;
//   unsigned char tempBlockBuffer[BLOCK_SIZE];
//
//   // es wird vom root-Inode ausgehend jedes Verzeichnis durchlaufen
//   // wenn der nächste Inode ein Directory ist, +1 und dieses Directory öffnen
//   // ansonsten für einen Daten-Inode +1
//   // muss in der main gemacht werden: initial den root mit += 64
//   mode = get4Bytes(p);
//   p += 32;
//
//   for (j = 0; j < 6; j++) {
//     addr = get4Bytes(p);
//     p += 4;
//     if (addr != 0) {
//       readBlock(f, addr, tempBlockBuffer);
//       inodesDirectoryBlock(tempBlockBuffer, f);
//     }
//
//     addr = get4Bytes(p);
//     p += 4;
//
//     if (addr != 0) {
//       readBlock(f, addr, tempBlockBuffer);
//       singleInodesDirectoryBlock(tempBlockBuffer, f);
//     }
//
//     addr = get4Bytes(p);
//     p += 4;
//
//     if (addr != 0) {
//       readBlock(f, addr, tempBlockBuffer);
//       doubleInodesDirectoryBlock(tempBlockBuffer, f);
//     }
//   }
//
//   return 1;
// }


void inodesDirectoryBlock(unsigned char *p, FILE *f) {
  EOS32_ino_t ino;
  int i;

  ino = get4Bytes(p);
  refs[ino].linkcount += 1;
  p += 64;

  ino = get4Bytes(p);
  refs[ino].linkcount += 1;
  p += 64;

  for (i = 2; i < DIRPB; i++) {
    ino = get4Bytes(p);

    if (ino != 0 && ino < iSize && refs[ino].flag == 0) {
      recursiveDirectory(ino, f);
    } else if (ino != 0 && ino < iSize && refs[ino].flag == 1) {
      refs[ino].linkcount += 1;
    }


    p += 64;
  }

  /*
    wenn im ersten if der Flag null ist erhöhe um eins
    a 85
    if 85
    b 85
    b 85
    b 85
    b 85
  */
  //
  // for (i = 0; i < DIRPB; i++) {
  //   ino = get4Bytes(p);
  //   printf("%d\n", ino);
  //   p += 4;
  //   if (ino != 0 && ino < iSize) {
  //     refs[ino] += 1;
  //     recursiveDirectory(ino, f);
  //   }
  //
  //   p += DIRSIZ;
  // }
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
