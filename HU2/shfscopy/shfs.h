typedef unsigned int EOS32_daddr_t;


// others
void error(char *fmt, ...);
void readBlock(FILE *disk, EOS32_daddr_t blockNum, unsigned char *blockBuffer);
unsigned int get4Bytes(unsigned char *addr);
int waitForReturn(void);
int checkBatch(int numLines);
void rawBlock(unsigned char *p);
void help(void);
int parseNumber(char **pc, unsigned int *pi);
void directoryBlock(unsigned char *p);

// freeBlocksTXT
void superBlock(unsigned char *p);
void freeBlock(unsigned char *p);
void filesystemSize(unsigned char *p);

// allInodesTXT
void inodeBlock(unsigned char *p);

// dataBlocksTXT
void datablocks(unsigned char *p, FILE *f);
void singleIndirectBlock(unsigned char *p);
void doubleIndirectBlock(unsigned char *p, FILE *f);

// inodesWithLinkcountZeroTXT
void inodesWithLinkcountZero(unsigned char *p);

// allInodesInDirectoriesTXT
void allInodesInDirectories(unsigned char *p, FILE *f);
void inodesDirectoryBlock(unsigned char *p);
void singleInodesDirectoryBlock(unsigned char *p, FILE *f);
void doubleInodesDirectoryBlock(unsigned char *p, FILE *f);

// open TXT + main
int main(int argc, char *argv[]);
void openAllInodesTXT();
void openAllInodesInDirectoriesTXT();
void openInodesWithLinkcountZeroTXT();

// check datasize
void datasize(unsigned char *p, FILE *f);
int singleIndirectBlockdata(unsigned char *p);
int doubleIndirectBlockdata(unsigned char *p, FILE *f);

// check if inode with linkcount 0 is free
void inodeIsFree(unsigned char *p);
