#ifndef _SHFS_H_
#define _SHFS_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <time.h>

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

#define IFMT		  070000	/* type of file */
#define IFREG		  040000	/* regular file */
#define IFDIR		  030000	/* directory */
#define IFCHR		  020000	/* character special */
#define IFBLK		  010000	/* block special */
#define IFFREE	  000000	/* reserved (indicates free inode) */
#define ISUID		  004000	/* set user id on execution */
#define ISGID		  002000	/* set group id on execution */
#define ISVTX		  001000	/* save swapped text even after use */
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

typedef struct {
	short dataList;
	short freeList;
} Block;

extern FILE *allInodesTXT;
extern EOS32_daddr_t linkBlock;
extern EOS32_daddr_t fsize;
extern int iSize;
extern int id;
extern Block *blocks;
extern short *refs;
extern unsigned int fsStart;


void readBlock(FILE *disk, EOS32_daddr_t blockNum, unsigned char *blockBuffer);
unsigned int get4Bytes(unsigned char *addr);
void filesystemSize(unsigned char *p);
void inodeBlock(unsigned char *p);
void error(char *fmt, int exitcode, ...);

#endif
