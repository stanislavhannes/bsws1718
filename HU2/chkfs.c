/*
 * chkfs.c -- check an EOS32 file system
 */

#include "shfs.h"
#include "chkfs.h"
#include "checkInodesAndBlocks.h"
#include "inodesInDir.h"
#include "blocksInFreelistAndInode.h"


FILE *allInodesTXT;
EOS32_daddr_t linkBlock;
EOS32_daddr_t fsize;
int iSize;
int id=-1;
Block *blocks;
Refs *refs;
unsigned int fsStart;

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
    printf("       <partition> is a partition number \n");
    exit(1);
  }
  disk = fopen(argv[1], "rb");
  if (disk == NULL) {
    error("cannot open disk image file '%s'", 2, argv[1]);
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
      error("illegal partition number '%s'", 4,  argv[2]);
    }
    fseek(disk, 1 * SECTOR_SIZE, SEEK_SET);
    if (fread(partTable, 1, SECTOR_SIZE, disk) != SECTOR_SIZE) {
      error("cannot read partition table of disk '%s'", 9, argv[1]);
    }
    ptptr = partTable + part * 32;
    partType = get4Bytes(ptptr + 0);
    if ((partType & 0x7FFFFFFF) != 0x00000058) {
      error("partition %d of disk '%s' does not contain an EOS32 file system", 5,
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
    error("file system has less than 2 blocks", 3);
  }

  currBlock = 1;
  readBlock(disk, currBlock, blockBuffer);
  filesystemSize(blockBuffer);

  blocks = (Block *) calloc(fsize, sizeof(Block));
  if (blocks == NULL){
    error("out of memory", 6);
  }

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

  refs = (Refs *) calloc(iSize, sizeof(Refs));
  if (refs == NULL){
    error("out of memory", 6);
  }

  /*for (i=2; i < 26; i++) {
    currBlock = i;
    readBlock(disk, currBlock, blockBuffer);
    allInodesInDirectories(blockBuffer, disk);
  }*/

  recursiveDirectory(1, disk);

  id = -1;

  for (i=2; i < 26; i++) {
    currBlock = i;
    readBlock(disk, currBlock, blockBuffer);
    checkLinkcount(blockBuffer);
  }

  free(refs);

  for (i=2; i < 26; i++) {
    currBlock = i;
    readBlock(disk, currBlock, blockBuffer);
    checkInodeMode(blockBuffer);
  }

  currBlock = 2;
  readBlock(disk, currBlock, blockBuffer);
  checkRootInode(blockBuffer);

  for (i=2; i < 26; i++) {
    currBlock = i;
    readBlock(disk, currBlock, blockBuffer);
    inodeIsFree(blockBuffer);
  }

  for (i=2; i < 26; i++) {
    currBlock = i;
    readBlock(disk, currBlock, blockBuffer);
    datasize(blockBuffer, disk);
  }

  fclose(disk);
  printf("File system check was successfully!\n");
  return 0;
}
