/*
 * shfs.c -- show an EOS32 file system
 */

#include "shfs.h"

FILE *allInodesTXT;
EOS32_daddr_t fsize;
int iSize;
unsigned int fsStart;

/*

Funktion von Geisse, ein kompletter Block (blockNum)
wird in blockBuffer geladen

*/
void readBlock(FILE *disk, EOS32_daddr_t blockNum, unsigned char *blockBuffer) {
  fseek(disk, fsStart * SECTOR_SIZE + blockNum * BLOCK_SIZE, SEEK_SET);
  if (fread(blockBuffer, BLOCK_SIZE, 1, disk) != 1) {
    error("cannot read block %lu (0x%lX)", 99, blockNum, blockNum);
  }
}

/*

Funktion von Geisse, gibt die nächsten 4 Bytes zurück, die Variable Blockbuffer
wird immer in 4 Byte Schritten gelesen, die ersten 4 Bytes sind zB in der Funktion
inodeBlock() die Variable mode, die nächsten 4 der linkcount usw...

*/
unsigned int get4Bytes(unsigned char *addr) {
  return (unsigned int) addr[0] << 24 |
         (unsigned int) addr[1] << 16 |
         (unsigned int) addr[2] <<  8 |
         (unsigned int) addr[3] <<  0;
}


/*

fsize = Anzahl der Blocks des kompletten FS, steht im Superblock,
iSize = Anzahl der Inodes eines EOS32-Diskimage

*/
void filesystemSize(unsigned char *p) {
  p += 4;
  fsize = get4Bytes(p);
  iSize = 24*64;
}

/*

Funktion von Geisse, so angepasst, dass die Ausgabe anstatt auf die Konsole
in die Datei allInodes.txt geschrieben wird. (quasi der Aufruf i für einen
Inode-Block)

*/
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


/*

Funktion von Geisse

*/
void error(char *fmt, int exitcode, ...) {
  va_list ap;

  va_start(ap, exitcode);
  printf("Error: ");
  vprintf(fmt, ap);
  printf("\n");
  va_end(ap);
  exit(exitcode);
}
