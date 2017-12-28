typedef unsigned int EOS32_daddr_t;

// others
void directoryBlock(unsigned char *p);
void error(char *fmt, ...);
void readBlock(FILE *disk, EOS32_daddr_t blockNum, unsigned char *blockBuffer);
unsigned int get4Bytes(unsigned char *addr);
int waitForReturn(void);
int checkBatch(int numLines);
void rawBlock(unsigned char *p);
void help(void);
int parseNumber(char **pc, unsigned int *pi);

// freelist.txt
void superBlock(unsigned char *p);
void freeBlock(unsigned char *p);

// inodelist.txt
void inodeBlock(unsigned char *p);

// datablockslist.txt
void datablocks(unsigned char *p, FILE *f);
void doubleIndirectBlock(unsigned char *p, FILE *f);
void singleIndirectBlock(unsigned char *p);

// control of datasize
void datasize(unsigned char *p, FILE *f);
int singleIndirectBlockdata(unsigned char *p);
int doubleIndirectBlockdata(unsigned char *p, FILE *f);

// open TXT + main
int main(int argc, char *argv[]);
void openFreelistTXT();
void openInodelistTXT();
void openinodesindirectoriesTXT();
void openDatablocksTXT();
void openInodeszerolcTXT();

// inodeszerolc.txt
void inodesZeroLC(unsigned char *p);

// inodesindirectories.txt
void inodesInDirectories(unsigned char *p, FILE *f);
void inodesDirectoryBlock(unsigned char *p);
void singleInodesDirectoryBlock(unsigned char *p, FILE *f);
void doubleInodesDirectoryBlock(unsigned char *p, FILE *f);
