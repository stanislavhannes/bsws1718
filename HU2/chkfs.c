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
short *refs;
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

  // start: Superblock wird eingelesen, filesystemSize wird bestimmt, blocks
  // wird mit diesen Variablen initialisiert
  currBlock = 1;
  readBlock(disk, currBlock, blockBuffer);
  filesystemSize(blockBuffer);
  blocks = malloc(fsize * (sizeof(Block)));
  if (blocks == NULL){
    error("out of memory", 6);
  }

  // der Superblock wird erneut von Beginn an eingelesen
  readBlock(disk, currBlock, blockBuffer);

  // und die erste Freiliste aus dem Superblock wird in der DS gespeichert
  superBlock(blockBuffer);

  // die Verlinkung wird so lange über die Funktion freeBlock() verfolgt, bis
  // der linkBlock 0 ist. currBlock wird = linkBlock gesetzt und dieser Block
  // wird über die Funktion readBlock in den blockBuffer gelesen
  //  => die Freiliste wird in der Datenstruktur blocks gespeichert
  while (linkBlock != 0) {
    currBlock = linkBlock;
    readBlock(disk, currBlock, blockBuffer);
    freeBlock(blockBuffer);
  }


  // die Datenblöcke werden aus den Inodes gelesen, und in der Datenstruktur
  // blocks gespeichert
  for (i=2; i < 26; i++) {
    currBlock = i;
    readBlock(disk, currBlock, blockBuffer);
    datablocks(blockBuffer, disk);
  }

  // stimmt die Datenstruktur oder gibt es Fehler?
  // siehe Aufgabenstellung, in jedem block steht genau eine "1"
  // der Index der Datenstruktur blocks ist die Blocknummer
  // glaube der Anfang muss noch ausgelassen werden, also die Blöcke 0 - 25
  blockCheck();

  free(blocks);

  // create allInodes.txt – visuelle Hilfe
  openAllInodesTXT();

  for (i=2; i < 26; i++) {
    currBlock = i;
    readBlock(disk, currBlock, blockBuffer);
    inodeBlock(blockBuffer);
  }

  fclose(allInodesTXT);


  // refs wird angelegt mit der Anzahl der Inodes, berechnet in filesystemSize()
  refs = malloc(iSize * sizeof(short));
  if (refs == NULL){
    error("out of memory", 6);
  }

  // Inode-Liste wird durchgegangen, siehe inodesInDir.c
  for (i=2; i < 26; i++) {
    currBlock = i;
    readBlock(disk, currBlock, blockBuffer);
    allInodesInDirectories(blockBuffer, disk);
  }

  // id wird auf -1 gesetzt
  id = -1;

  // checkLinkcount überprüft, ob der Linkcount im inode mit der Anzahl der gefunden
  // Verweisen von allInodesInDirectories() übereinstimmt
  for (i=2; i < 26; i++) {
    currBlock = i;
    readBlock(disk, currBlock, blockBuffer);
    checkLinkcount(blockBuffer);
  }

  free(refs);

  fclose(disk);
  printf("File system check was successfully!\n");
  return 0;
}

/*

Dient zur Hilfe, um visuell alle Inodes zu checken

*/
void openAllInodesTXT() {
  allInodesTXT = fopen("allInodes.txt", "w+");

  if(allInodesTXT == NULL) {
    error("couldn't open allInodes.txt", 9);
  }
}
