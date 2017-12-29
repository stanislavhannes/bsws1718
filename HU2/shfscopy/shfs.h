typedef unsigned int EOS32_daddr_t;

// checks the inodes and blocks
void blockCheck();
void checkLinkcount(unsigned char *p);
void inodeIsFree(unsigned char *p);
void datasize(unsigned char *p, FILE *f);
int singleIndirectBlockdata(unsigned char *p);
int doubleIndirectBlockdata(unsigned char *p, FILE *f);

// inodes in directories
void allInodesInDirectories(unsigned char *p, FILE *f);
void inodesDirectoryBlock(unsigned char *p);
void singleInodesDirectoryBlock(unsigned char *p, FILE *f);
void doubleInodesDirectoryBlock(unsigned char *p, FILE *f);

// main + others
int main(int argc, char *argv[]);
void openAllInodesTXT();
void readBlock(FILE *disk, EOS32_daddr_t blockNum, unsigned char *blockBuffer);
unsigned int get4Bytes(unsigned char *addr);
void filesystemSize(unsigned char *p);
void inodeBlock(unsigned char *p);
void error(char *fmt, ...);

// all blocks on the freelist and in Inodes
void superBlock(unsigned char *p);
void datablocks(unsigned char *p, FILE *f);
void freeBlock(unsigned char *p);
void singleIndirectBlock(unsigned char *p);
void doubleIndirectBlock(unsigned char *p, FILE *f);
