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

FILE *freelist;
FILE *inodelist;
FILE *datablockslist;
FILE *singleindirectlist;
FILE *doubleindirectlist;
FILE *inodesindirectories;
FILE *inodeszerolc;

EOS32_daddr_t linkblock;
int id = -1;
// gcc -Wall -o shfs shfs.c

unsigned int fsStart;

void inodesZeroLC(unsigned char *p) {
  unsigned int nlink;
  int i;

  for (i = 0; i < INOPB; i++) {

    p += 4;
    id++;

    nlink = get4Bytes(p);
    if (nlink == 0 && id > 0) {
      fprintf(inodeszerolc, "%d\n", id);
    }

    p += 60;
  }
}

void inodesInDirectories(unsigned char *p, FILE *f) {
  unsigned int mode;
  EOS32_daddr_t addr;
  int i, j;
  unsigned char datablockBuffer[BLOCK_SIZE];

  //checkBatch(0);
  for (i = 0; i < INOPB; i++) {
    mode = get4Bytes(p);
    p += 32;
    id++;
    if (mode != 0 && (mode & IFMT) == IFDIR) {

      for (j = 0; j < 6; j++) {
        addr = get4Bytes(p);
        p += 4;
        if (mode != 0 && addr != 0) {
          readBlock(f, addr, datablockBuffer);
          inodesDirectoryBlock(datablockBuffer);
          //if (checkBatch(1)) return;
        }
      }
      addr = get4Bytes(p);
      p += 4;
      if (mode != 0) {

        if (addr != 0) {
          readBlock(f, addr, datablockBuffer);
          singleInodesDirectoryBlock(datablockBuffer, f);
        }
      }
      addr = get4Bytes(p);
      p += 4;
      if (mode != 0) {

        if (addr != 0) {
          readBlock(f, addr, datablockBuffer);
          doubleInodesDirectoryBlock(datablockBuffer, f);
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
    // fprintf(inodesindirectories, "%02d:  ", i);
    ino = get4Bytes(p);
    p += 4;
    if (ino != 0) {
      fprintf(inodesindirectories, "%u\n", ino);
    }


    //if (checkBatch(1)) return;
    // fprintf(inodesindirectories, "     name  = ");
    /*if (*p == '\0') {
      // fprintf(inodesindirectories, "<empty>");
    } else {
      for (j = 0; j < DIRSIZ; j++) {
        c = *(p + j);
        if (c == '\0') {
          break;
        }
        if (c < 0x20 || c >= 0x7F) {
          // fprintf(inodesindirectories, ".");
        } else {
          // fprintf(inodesindirectories, "%c", c);
        }
      }
    }
    // fprintf(inodesindirectories, "\n");
    //if (checkBatch(1)) return;*/
    p += DIRSIZ;
  }
}

void singleInodesDirectoryBlock(unsigned char *p, FILE *f) {
  EOS32_daddr_t addr;
  int i;
  unsigned char datablockBuffer[BLOCK_SIZE];

  for (i = 0; i < BLOCK_SIZE / sizeof(EOS32_daddr_t); i++) {
    addr = get4Bytes(p);
    p += 4;
    if (addr != 0) {
      readBlock(f, addr, datablockBuffer);
      inodesDirectoryBlock(datablockBuffer);
    }
  }
}

void doubleInodesDirectoryBlock(unsigned char *p, FILE *f) {
  EOS32_daddr_t addr;
  int i;
  unsigned char datablockBuffer[BLOCK_SIZE];

  for (i = 0; i < BLOCK_SIZE / sizeof(EOS32_daddr_t); i++) {
    addr = get4Bytes(p);
    p += 4;
    if (addr != 0) {
      readBlock(f, addr, datablockBuffer);
      singleInodesDirectoryBlock(datablockBuffer, f);
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
  //char line[LINE_SIZE];
  int quit;
  int i;
  //char *p;
  //unsigned int n;

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
  help();
  quit = 0;

  // create freelist.txt
  openFreelistTXT();
  superBlock(blockBuffer);

  while (linkblock != 0) {
    currBlock = linkblock;
    readBlock(disk, currBlock, blockBuffer);
    freeBlock(blockBuffer);
  }

  // end of file (EOF == -1)
  // fprintf(freelist, "-1");
  // fclose(freelist);


  //create inodelist.txt
  openInodelistTXT();

  for (i=2; i < 26; i++) {
    currBlock = i;
    readBlock(disk, currBlock, blockBuffer);
    inodeBlock(blockBuffer);
  }

  fclose(inodelist);

  // create datablockslist.txt
  openDatablocksTXT();

  for (i=2; i < 26; i++) {
    currBlock = i;
    readBlock(disk, currBlock, blockBuffer);
    datablocks(blockBuffer, disk);
  }



  // all double indirect blocks to the singleindirectlist
  /*char * line = NULL;
  size_t len = 0;
  ssize_t read;
  int num;

  fseek(doubleindirectlist, 0, SEEK_SET);

  while ((read = getline(&line, &len, singleindirectlist)) != -1) {
    num = atoi(line);
    currBlock = num;
    readBlock(disk, currBlock, blockBuffer);
    doubleIndirectBlock(blockBuffer);
  }

  // get all datablocks from singleIndirectBlocks
  line = NULL;
  len = 0;

  fseek(singleindirectlist, 0, SEEK_SET);

  while ((read = getline(&line, &len, singleindirectlist)) != -1) {
    num = atoi(line);
    currBlock = num;
    readBlock(disk, currBlock, blockBuffer);
    singleIndirectBlock(blockBuffer);
  }*/

  fclose(datablockslist);
  // fclose(singleindirectlist);
  // fclose(doubleindirectlist);

  openinodesindirectoriesTXT();

  for (i=2; i < 26; i++) {
    currBlock = i;
    readBlock(disk, currBlock, blockBuffer);
    inodesInDirectories(blockBuffer, disk);
  }

  openInodeszerolcTXT();
  id = -1;

  for (i=2; i < 26; i++) {
    currBlock = i;
    readBlock(disk, currBlock, blockBuffer);
    inodesZeroLC(blockBuffer);
  }

  fclose(inodeszerolc);

  /*currBlock = 745;
  readBlock(disk, currBlock, blockBuffer);
  directoryBlock(blockBuffer);*/

  fclose(inodesindirectories);

  // e) Die Groesse einer Datei ist nicht konsistent mit den im
  // Inode vermerkten Bloecken: Exit-Code 14.
  // circa 4096 Bytes pro Block
  //
  // also Bloecke im Inode zählen und mit der Dateigroeße vergleichen
  // dazu ist keine Datenstruktur nötig, Programm kann abgebrochen werden

  // im inode erst die anzahl der Blöcke zählen, auch si und di mit Hilfe der
  // Funktionen aus a-d, danach dann mal 4096 und Wert mit size
  // vergleichen, danach beide in Datei schreiben
  // neue Datei mit den entsprechenden Funktionen anlegen

  /*while (!quit) {
    printf("shfs [block %u (0x%X)] > ", currBlock, currBlock);
    fflush(stdout);
    if (fgets(line, LINE_SIZE, stdin) == NULL) {
      printf("\n");
      break;
    }
    if (line[0] == '\0' || line[0] == '\n') {
      continue;
    }
    switch (line[0]) {
      case 'h':
      case '?':
        help();
        break;
      case 'q':
        quit = 1;
        break;
      case 'r':
        rawBlock(blockBuffer);
        break;
      case 's':
        superBlock(blockBuffer);
        break;
      case 'i':
        inodeBlock(blockBuffer);
        break;
      case 'd':
        directoryBlock(blockBuffer);
        break;
      case 'f':
        freeBlock(blockBuffer);
        break;
      case '*':
        indirectBlock(blockBuffer);
        break;
      case 'b':
        p = line + 1;
        if (!parseNumber(&p, &n)) {
          break;
        }
        if (*p != '\0' && *p != '\n') {
          printf("Error: cannot parse block number!\n");
          break;
        }
        if (n >= numBlocks) {
          printf("Error: block number too big for file system!\n");
          break;
        }
        currBlock = n;
        readBlock(disk, currBlock, blockBuffer);
        break;
      case '+':
        n = currBlock + 1;
        if (n >= numBlocks) {
          printf("Error: block number too big for file system!\n");
          break;
        }
        currBlock = n;
        readBlock(disk, currBlock, blockBuffer);
        break;
      case '-':
        n = currBlock - 1;
        if (n >= numBlocks) {
          printf("Error: block number too big for file system!\n");
          break;
        }
        currBlock = n;
        readBlock(disk, currBlock, blockBuffer);
        break;
      case 't':
        p = line + 1;
        if (!parseNumber(&p, &n)) {
          break;
        }
        if (*p != '\0' && *p != '\n') {
          printf("Error: cannot parse inode number!\n");
          break;
        }
        printf("inode %u (0x%X) is in block %u (0x%X), inode %u\n",
               n, n, n / INOPB + 2, n / INOPB + 2, n % INOPB);
        break;
      default:
        printf("Unknown command, type 'h' for help!\n");
        break;
    }
  }*/

  fclose(disk);
  return 0;
}

void openFreelistTXT() {

  freelist = fopen("freelist.txt", "w+");

  if(freelist == NULL) {
  	printf("Datei konnte nicht geoeffnet werden.\n");
  }
}

void openInodelistTXT() {
  inodelist = fopen("inodelist.txt", "w+");

  if(inodelist == NULL) {
  	printf("Datei konnte nicht geoeffnet werden.\n");
  }
}

void openDatablocksTXT() {
  datablockslist = fopen("datablockslist.txt", "w+");

  if(datablockslist == NULL) {
  	printf("Datei konnte nicht geoeffnet werden.\n");
  }
}

void openinodesindirectoriesTXT() {

  inodesindirectories = fopen("inodesindirectories.txt", "w+");

  if(inodesindirectories == NULL) {
  	printf("Datei konnte nicht geoeffnet werden.\n");
  }
}

void openInodeszerolcTXT() {

  inodeszerolc = fopen("inodeszerolc.txt", "w+");

  if(inodeszerolc == NULL) {
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
    if (size > calcSize || size < (calcSize-BLOCK_SIZE)) {
      // TODO: error
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

void error(char *fmt, ...) {
  va_list ap;

  va_start(ap, fmt);
  printf("Error: ");
  vprintf(fmt, ap);
  printf("\n");
  va_end(ap);
  exit(1);
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


int waitForReturn(void) {
  char line[LINE_SIZE];

  printf("press <enter> to continue, <esc> to break, q to quit: ");
  fflush(stdout);
  if (fgets(line, LINE_SIZE, stdin) == NULL) {
    printf("\n");
    exit(0);
  }
  if (line[0] == 'q') {
    exit(0);
  }
  if (line[0] == 0x1B) {
    return 1;
  }
  return 0;
}


int checkBatch(int numLines) {
  static int lines;
  int r;

  r = 0;
  if (numLines == 0) {
    /* initialize */
    lines = 0;
  } else {
    /* output numLines lines */
    lines += numLines;
    if (lines >= LINES_PER_BATCH) {
      r = waitForReturn();
      lines = 0;
    }
  }
  return r;
}


void rawBlock(unsigned char *p) {
  int i, j;
  unsigned char c;

  checkBatch(0);
  for (i = 0; i < BLOCK_SIZE / 16; i++) {
    printf("%04X   ", i * 16);
    for (j = 0; j < 16; j++) {
      c = p[i * 16 + j];
      printf("%02X ", c);
    }
    printf("   ");
    for (j = 0; j < 16; j++) {
      c = p[i * 16 + j];
      if (c < 0x20 || c >= 0x7F) {
        printf(".");
      } else {
        printf("%c", c);
      }
    }
    printf("\n");
    if (checkBatch(1)) return;
  }
}


void superBlock(unsigned char *p) {

  unsigned int nfree;
  EOS32_daddr_t free;
  int i;

  p += 4;
  p += 4;
  p += 4;
  p += 4;
  p += 4;
  p += 4;

  // for (i = 0; i < NICINOD; i++) { p += 4; }
  p = p + (NICINOD * 4);

  nfree = get4Bytes(p);
  p += 4;
  if (checkBatch(1)) return;
  for (i = 0; i < NICFREE; i++) {
    free = get4Bytes(p);
    p += 4;
    if (i == 0) {
      linkblock = free;
    } else {
        if (i < nfree) {
          fprintf(freelist, "%u\n", free);
        }
      }
  }
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

  //checkBatch(0);
  for (i = 0; i < INOPB; i++) {
    fprintf(inodelist, "inode %d:", i);
    mode = get4Bytes(p);
    p += 4;
    if (mode != 0) {
      fprintf(inodelist, "  type/mode = 0x%08X = ", mode);
      if ((mode & IFMT) == IFREG) {
        fprintf(inodelist, "-");
      } else
      if ((mode & IFMT) == IFDIR) {
        fprintf(inodelist, "d");
      } else
      if ((mode & IFMT) == IFCHR) {
        fprintf(inodelist, "c");
      } else
      if ((mode & IFMT) == IFBLK) {
        fprintf(inodelist, "b");
      } else {
        fprintf(inodelist, "?");
      }
      fprintf(inodelist, "%c", mode & IUREAD  ? 'r' : '-');
      fprintf(inodelist, "%c", mode & IUWRITE ? 'w' : '-');
      fprintf(inodelist, "%c", mode & IUEXEC  ? 'x' : '-');
      fprintf(inodelist, "%c", mode & IGREAD  ? 'r' : '-');
      fprintf(inodelist, "%c", mode & IGWRITE ? 'w' : '-');
      fprintf(inodelist, "%c", mode & IGEXEC  ? 'x' : '-');
      fprintf(inodelist, "%c", mode & IOREAD  ? 'r' : '-');
      fprintf(inodelist, "%c", mode & IOWRITE ? 'w' : '-');
      fprintf(inodelist, "%c", mode & IOEXEC  ? 'x' : '-');
      if (mode & ISUID) {
        fprintf(inodelist, ", set uid");
      }
      if (mode & ISGID) {
        fprintf(inodelist, ", set gid");
      }
      if (mode & ISVTX) {
        fprintf(inodelist, ", save text");
      }
    } else {
      fprintf(inodelist, "  <free>");
    }
    fprintf(inodelist, "\n");
    // if (checkBatch(1)) return;
    nlink = get4Bytes(p);
    p += 4;
    if (mode != 0) {
      fprintf(inodelist, "  nlnk = %u (0x%08X)\n", nlink, nlink);
      //if (checkBatch(1)) return;
    }
    uid = get4Bytes(p);
    p += 4;
    if (mode != 0) {
      fprintf(inodelist, "  uid  = %u (0x%08X)\n", uid, uid);
      //if (checkBatch(1)) return;
    }
    gid = get4Bytes(p);
    p += 4;
    if (mode != 0) {
      fprintf(inodelist, "  gid  = %u (0x%08X)\n", gid, gid);
      //if (checkBatch(1)) return;
    }
    tim = get4Bytes(p);
    p += 4;
    dat = ctime((time_t *) &tim);
    dat[strlen(dat) - 1] = '\0';
    if (mode != 0) {
      fprintf(inodelist, "  time inode created = %d (%s)\n", tim, dat);
      //if (checkBatch(1)) return;
    }
    tim = get4Bytes(p);
    p += 4;
    dat = ctime((time_t *) &tim);
    dat[strlen(dat) - 1] = '\0';
    if (mode != 0) {
      fprintf(inodelist, "  time last modified = %d (%s)\n", tim, dat);
      //if (checkBatch(1)) return;
    }
    tim = get4Bytes(p);
    p += 4;
    dat = ctime((time_t *) &tim);
    dat[strlen(dat) - 1] = '\0';
    if (mode != 0) {
      fprintf(inodelist, "  time last accessed = %d (%s)\n", tim, dat);
      //if (checkBatch(1)) return;
    }
    size = get4Bytes(p);
    p += 4;
    if (mode != 0) {
      fprintf(inodelist, "  size = %u (0x%X)\n", size, size);
      //if (checkBatch(1)) return;
    }
    for (j = 0; j < 6; j++) {
      addr = get4Bytes(p);
      p += 4;
      if (mode != 0) {
        fprintf(inodelist, "  direct block[%1d] = %u (0x%X)\n", j, addr, addr);
        //if (checkBatch(1)) return;
      }
    }
    addr = get4Bytes(p);
    p += 4;
    if (mode != 0) {
      fprintf(inodelist, "  single indirect = %u (0x%X)\n", addr, addr);
      //if (checkBatch(1)) return;
    }
    addr = get4Bytes(p);
    p += 4;
    if (mode != 0) {
      fprintf(inodelist, "  double indirect = %u (0x%X)\n", addr, addr);
      //if (checkBatch(1)) return;
    }
  }
}

void datablocks(unsigned char *p, FILE *f) {
  unsigned int mode;
  EOS32_daddr_t addr;
  int i, j;
  unsigned char datablockBuffer[BLOCK_SIZE];

  for (i = 0; i < INOPB; i++) {

    mode = get4Bytes(p);
    p += 32;

    for (j = 0; j < 6; j++) {
      addr = get4Bytes(p);
      p += 4;
      if (mode != 0 && addr != 0) {
        fprintf(datablockslist, "%u\n", addr);
      }
    }
    addr = get4Bytes(p);
    p += 4;
    if (mode != 0) {

      if (addr != 0) {
        fprintf(datablockslist, "%u\n", addr);
        readBlock(f, addr, datablockBuffer);
        singleIndirectBlock(datablockBuffer);
      }
    }
    addr = get4Bytes(p);
    p += 4;
    if (mode != 0) {

      if (addr != 0) {
        fprintf(datablockslist, "%u\n", addr);
        readBlock(f, addr, datablockBuffer);
        doubleIndirectBlock(datablockBuffer, f);
      }
    }
  }
}

void directoryBlock(unsigned char *p) {
  EOS32_ino_t ino;
  int i, j;
  unsigned char c;

  //checkBatch(0);
  for (i = 0; i < DIRPB; i++) {
    fprintf(inodesindirectories, "%02d:  ", i);
    ino = get4Bytes(p);
    p += 4;
    fprintf(inodesindirectories, "inode = %u (0x%X)\n", ino, ino);
    //if (checkBatch(1)) return;
    fprintf(inodesindirectories, "     name  = ");
    if (*p == '\0') {
      fprintf(inodesindirectories, "<empty>");
    } else {
      for (j = 0; j < DIRSIZ; j++) {
        c = *(p + j);
        if (c == '\0') {
          break;
        }
        if (c < 0x20 || c >= 0x7F) {
          fprintf(inodesindirectories, ".");
        } else {
          fprintf(inodesindirectories, "%c", c);
        }
      }
    }
    fprintf(inodesindirectories, "\n");
    //if (checkBatch(1)) return;
    p += DIRSIZ;
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
      linkblock = addr;
    } else {
        if (i < nfree) {
          fprintf(freelist, "%u\n", addr);
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
      fprintf(datablockslist, "%u\n", addr);
    }
  }
}

void doubleIndirectBlock(unsigned char *p, FILE *f) {
  EOS32_daddr_t addr;
  int i;
  unsigned char datablockBuffer[BLOCK_SIZE];

  for (i = 0; i < BLOCK_SIZE / sizeof(EOS32_daddr_t); i++) {
    addr = get4Bytes(p);
    p += 4;
    if (addr != 0) {
      readBlock(f, addr, datablockBuffer);
      singleIndirectBlock(datablockBuffer);
    }
  }
}


void help(void) {
  printf("Commands are:\n");
  printf("  h        help\n");
  printf("  q        quit\n");
  printf("  r        show block as raw data\n");
  printf("  s        show block as super block\n");
  printf("  i        show block as inode block\n");
  printf("  d        show block as directory block\n");
  printf("  f        show block as block on the free list\n");
  printf("  *        show block as indirect block\n");
  printf("  b <num>  set current block to <num>\n");
  printf("  +        increment current block\n");
  printf("  -        decrement current block\n");
  printf("  t <num>  translate inode <num> to block number\n");
  printf("           and relative inode number within block\n");
}


int parseNumber(char **pc, unsigned int *pi) {
  char *p;
  unsigned int base, dval;
  unsigned int n;

  p = *pc;
  while (*p == ' ' || *p == '\t') {
    p++;
  }
  if (*p == '\0' || *p == '\n') {
    printf("Error: number is missing!\n");
    return 0;
  }
  base = 10;
  if (*p == '0') {
    p++;
    if (*p != '\0' && *p != '\n') {
      if (*p == 'x' || *p == 'X') {
        base = 16;
        p++;
      } else {
        base = 8;
      }
    }
  }
  n = 0;
  while ((*p >= '0' && *p <= '9') ||
         (*p >= 'a' && *p <= 'f') ||
         (*p >= 'A' && *p <= 'F')) {
    if (*p >= '0' && *p <= '9') {
      dval = (*p - '0');
    } else
    if (*p >= 'a' && *p <= 'f') {
      dval = (*p - 'a' + 10);
    } else
    if (*p >= 'A' && *p <= 'F') {
      dval = (*p - 'A' + 10);
    }
    if (dval >= base) {
      printf("Error: digit value %d is illegal in number base %d\n",
             dval, base);
      return 0;
    }
    n *= base;
    n += dval;
    p++;
  }
  while (*p == ' ' || *p == '\t') {
    p++;
  }
  *pc = p;
  *pi = n;
  return 1;
}
