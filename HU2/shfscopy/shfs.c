/*
 * shfs.c -- show an EOS32 file system
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <time.h>

#include "shfs.h"


#define SECTOR_SIZE	512	/* disk sector size in bytes */
#define BLOCK_SIZE	4096	/* disk block size in bytes */
#define SPB		(BLOCK_SIZE / SECTOR_SIZE)
#define LINE_SIZE	100	/* input line buffer size in bytes */
#define LINES_PER_BATCH	32	/* number of lines output in one batch */

#define NICINOD		500	/* number of free inodes in superblock */
#define NICFREE		500	/* number of free blocks in superblock */
#define INOPB		64	/* number of inodes per block */
#define DIRPB		64	/* number of directory entries per block */
#define DIRSIZ		60	/* max length of path name component */

#define IFMT		070000	/* type of file */
#define   IFREG		040000	/* regular file */
#define   IFDIR		030000	/* directory */
#define   IFCHR		020000	/* character special */
#define   IFBLK		010000	/* block special */
#define   IFFREE	000000	/* reserved (indicates free inode) */
#define ISUID		004000	/* set user id on execution */
#define ISGID		002000	/* set group id on execution */
#define ISVTX		001000	/* save swapped text even after use */
#define IUREAD		000400	/* user's read permission */
#define IUWRITE		000200	/* user's write permission */
#define IUEXEC		000100	/* user's execute permission */
#define IGREAD		000040	/* group's read permission */
#define IGWRITE		000020	/* group's write permission */
#define IGEXEC		000010	/* group's execute permission */
#define IOREAD		000004	/* other's read permission */
#define IOWRITE		000002	/* other's write permission */
#define IOEXEC		000001	/* other's execute permission */

typedef unsigned int EOS32_ino_t;
typedef unsigned int EOS32_daddr_t;
typedef unsigned int EOS32_off_t;
typedef int EOS32_time_t;

FILE *allInodesTXT;

EOS32_daddr_t linkBlock;
EOS32_daddr_t fsize;
int iSize;
int id = -1;

typedef struct {
	short dataList;
	short freeList;
} Block;
// gcc -Wall -o shfs shfs.c

Block *blocks;
short *refs;

unsigned int fsStart;

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
    if (ino != 0) {
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

int main(int argc, char *argv[]) {
  FILE *disk;
  unsigned int fsSize;
  int part;
  char *endptr;
  unsigned char partTable[SECTOR_SIZE];
  unsigned char *ptptr;
  unsigned int partType;
  EOS32_daddr_t numBlocks;
  EOS32_daddr_t currBlock;
  unsigned char blockBuffer[BLOCK_SIZE];
  int i;


  if (argc != 3) {
    printf("Usage: %s <disk> <partition>\n", argv[0]);
    printf("       <disk> is a disk image file name\n");
    printf("       <partition> is a partition number ");
    printf("(or '*' for the whole disk)\n");
    exit(1);
  }
  disk = fopen(argv[1], "rb");
  if (disk == NULL) {
    error("cannot open disk image file '%s'", argv[1]);
  }
  if (strcmp(argv[2], "*") == 0) {
    /* whole disk contains one single file system */
    fsStart = 0;
    fseek(disk, 0, SEEK_END);
    fsSize = ftell(disk) / SECTOR_SIZE;
  } else {
    /* argv[2] is partition number of file system */
    part = strtoul(argv[2], &endptr, 10);
    if (*endptr != '\0' || part < 0 || part > 15) {
      error("illegal partition number '%s'", argv[2]);
    }
    fseek(disk, 1 * SECTOR_SIZE, SEEK_SET);
    if (fread(partTable, 1, SECTOR_SIZE, disk) != SECTOR_SIZE) {
      error("cannot read partition table of disk '%s'", argv[1]);
    }
    ptptr = partTable + part * 32;
    partType = get4Bytes(ptptr + 0);
    if ((partType & 0x7FFFFFFF) != 0x00000058) {
      error("partition %d of disk '%s' does not contain an EOS32 file system",
            part, argv[1]);
    }
    fsStart = get4Bytes(ptptr + 4);
    fsSize = get4Bytes(ptptr + 8);
  }
  printf("File system has size %u (0x%X) sectors of %d bytes each.\n",
         fsSize, fsSize, SECTOR_SIZE);
  if (fsSize % SPB != 0) {
    printf("File system size is not a multiple of block size.\n");
  }
  numBlocks = fsSize / SPB;
  printf("This equals %u (0x%X) blocks of %d bytes each.\n",
         numBlocks, numBlocks, BLOCK_SIZE);
  if (numBlocks < 2) {
    error("file system has less than 2 blocks");
  }

  // start
  currBlock = 1;
  readBlock(disk, currBlock, blockBuffer);
  filesystemSize(blockBuffer);
  blocks = malloc(fsize * (sizeof(Block)));

  readBlock(disk, currBlock, blockBuffer);

  superBlock(blockBuffer);

  while (linkBlock != 0) {
    currBlock = linkBlock;
    readBlock(disk, currBlock, blockBuffer);
    freeBlock(blockBuffer);
  }

  for (i=2; i < 26; i++) {
    currBlock = i;
    readBlock(disk, currBlock, blockBuffer);
    datablocks(blockBuffer, disk);
  }

  blockCheck();

  free(blocks);

  // create allInodes.txt
  openAllInodesTXT();

  for (i=2; i < 26; i++) {
    currBlock = i;
    readBlock(disk, currBlock, blockBuffer);
    inodeBlock(blockBuffer);
  }

  fclose(allInodesTXT);

  refs = malloc(iSize * sizeof(short));

  for (i=2; i < 26; i++) {
    currBlock = i;
    readBlock(disk, currBlock, blockBuffer);
    allInodesInDirectories(blockBuffer, disk);
  }

  id = -1;

  for (i=2; i < 26; i++) {
    currBlock = i;
    readBlock(disk, currBlock, blockBuffer);
    checkLinkcount(blockBuffer);
  }

  printf("%d %d\n", iSize, fsize);

  free(refs);

  fclose(disk);
  return 0;
}


void openAllInodesTXT() {
  allInodesTXT = fopen("allInodes.txt", "w+");

  if(allInodesTXT == NULL) {
  	printf("Datei konnte nicht geoeffnet werden.\n");
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


void readBlock(FILE *disk, EOS32_daddr_t blockNum, unsigned char *blockBuffer) {
  fseek(disk, fsStart * SECTOR_SIZE + blockNum * BLOCK_SIZE, SEEK_SET);
  if (fread(blockBuffer, BLOCK_SIZE, 1, disk) != 1) {
    error("cannot read block %lu (0x%lX)", blockNum, blockNum);
  }
}


unsigned int get4Bytes(unsigned char *addr) {
  return (unsigned int) addr[0] << 24 |
         (unsigned int) addr[1] << 16 |
         (unsigned int) addr[2] <<  8 |
         (unsigned int) addr[3] <<  0;
}


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

void filesystemSize(unsigned char *p) {
  p += 4;
  fsize = get4Bytes(p);
  iSize = 24*64;
}


void inodeBlock(unsigned char *p) {
  unsigned int mode;
  unsigned int nlink;
  unsigned int uid;
  unsigned int gid;
  EOS32_time_t tim;
  char *dat;
  EOS32_off_t size;
  EOS32_daddr_t addr;
  int i, j;

  for (i = 0; i < INOPB; i++) {
    fprintf(allInodesTXT, "inode %d:", i);
    mode = get4Bytes(p);
    p += 4;
    if (mode != 0) {
      fprintf(allInodesTXT, "  type/mode = 0x%08X = ", mode);
      if ((mode & IFMT) == IFREG) {
        fprintf(allInodesTXT, "-");
      } else
      if ((mode & IFMT) == IFDIR) {
        fprintf(allInodesTXT, "d");
      } else
      if ((mode & IFMT) == IFCHR) {
        fprintf(allInodesTXT, "c");
      } else
      if ((mode & IFMT) == IFBLK) {
        fprintf(allInodesTXT, "b");
      } else {
        fprintf(allInodesTXT, "?");
      }
      fprintf(allInodesTXT, "%c", mode & IUREAD  ? 'r' : '-');
      fprintf(allInodesTXT, "%c", mode & IUWRITE ? 'w' : '-');
      fprintf(allInodesTXT, "%c", mode & IUEXEC  ? 'x' : '-');
      fprintf(allInodesTXT, "%c", mode & IGREAD  ? 'r' : '-');
      fprintf(allInodesTXT, "%c", mode & IGWRITE ? 'w' : '-');
      fprintf(allInodesTXT, "%c", mode & IGEXEC  ? 'x' : '-');
      fprintf(allInodesTXT, "%c", mode & IOREAD  ? 'r' : '-');
      fprintf(allInodesTXT, "%c", mode & IOWRITE ? 'w' : '-');
      fprintf(allInodesTXT, "%c", mode & IOEXEC  ? 'x' : '-');
      if (mode & ISUID) {
        fprintf(allInodesTXT, ", set uid");
      }
      if (mode & ISGID) {
        fprintf(allInodesTXT, ", set gid");
      }
      if (mode & ISVTX) {
        fprintf(allInodesTXT, ", save text");
      }
    } else {
      fprintf(allInodesTXT, "  <free>");
    }
    fprintf(allInodesTXT, "\n");
    nlink = get4Bytes(p);
    p += 4;
    if (mode != 0) {
      fprintf(allInodesTXT, "  nlnk = %u (0x%08X)\n", nlink, nlink);
    }
    uid = get4Bytes(p);
    p += 4;
    if (mode != 0) {
      fprintf(allInodesTXT, "  uid  = %u (0x%08X)\n", uid, uid);
    }
    gid = get4Bytes(p);
    p += 4;
    if (mode != 0) {
      fprintf(allInodesTXT, "  gid  = %u (0x%08X)\n", gid, gid);
    }
    tim = get4Bytes(p);
    p += 4;
    dat = ctime((time_t *) &tim);
    dat[strlen(dat) - 1] = '\0';
    if (mode != 0) {
      fprintf(allInodesTXT, "  time inode created = %d (%s)\n", tim, dat);
    }
    tim = get4Bytes(p);
    p += 4;
    dat = ctime((time_t *) &tim);
    dat[strlen(dat) - 1] = '\0';
    if (mode != 0) {
      fprintf(allInodesTXT, "  time last modified = %d (%s)\n", tim, dat);
    }
    tim = get4Bytes(p);
    p += 4;
    dat = ctime((time_t *) &tim);
    dat[strlen(dat) - 1] = '\0';
    if (mode != 0) {
      fprintf(allInodesTXT, "  time last accessed = %d (%s)\n", tim, dat);
    }
    size = get4Bytes(p);
    p += 4;
    if (mode != 0) {
      fprintf(allInodesTXT, "  size = %u (0x%X)\n", size, size);
    }
    for (j = 0; j < 6; j++) {
      addr = get4Bytes(p);
      p += 4;
      if (mode != 0) {
        fprintf(allInodesTXT, "  direct block[%1d] = %u (0x%X)\n", j, addr, addr);
      }
    }
    addr = get4Bytes(p);
    p += 4;
    if (mode != 0) {
      fprintf(allInodesTXT, "  single indirect = %u (0x%X)\n", addr, addr);
    }
    addr = get4Bytes(p);
    p += 4;
    if (mode != 0) {
      fprintf(allInodesTXT, "  double indirect = %u (0x%X)\n", addr, addr);
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

void error(char *fmt, ...) {
  va_list ap;

  va_start(ap, fmt);
  printf("Error: ");
  vprintf(fmt, ap);
  printf("\n");
  va_end(ap);
  exit(1);
}
