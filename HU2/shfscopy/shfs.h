typedef unsigned int EOS32_daddr_t;

void error(char *fmt, ...);
void readBlock(FILE *disk, EOS32_daddr_t blockNum, unsigned char *blockBuffer);
unsigned int get4Bytes(unsigned char *addr);
int waitForReturn(void);
int checkBatch(int numLines);
void rawBlock(unsigned char *p);
void superBlock(unsigned char *p);
void inodeBlock(unsigned char *p);
void directoryBlock(unsigned char *p);
void freeBlock(unsigned char *p);
void indirectBlock(unsigned char *p);
void help(void);
int parseNumber(char **pc, unsigned int *pi);
int main(int argc, char *argv[]);
void openFreelistTXT();
void openInodelistTXT();
