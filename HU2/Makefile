CFLAGS= -g -Wall -ansi -pedantic -std=c99
CC = gcc
CSOURCES= shfs.c checkInodesAndBlocks.c inodesInDir.c blocksInFreelistAndInode.c chkfs.c 

all: 		hu2

hu2	: 		$(CSOURCES)
			$(CC) $(CFLAGS) -o $@ $^

%.o: 		%.c
			$(CC) $(CFLAGS) -o $@ -c $<

depend.mak: $(CSOURCES)
			gcc -MM $^ > $@  #Schreibt Lokale abhaengigkeiten in depend.mak

-include depend.mak  

dist-clean: clean
			rm -f hu2 depend.mak
			rm -f *.txt

clean :		
			rm -f *.o
			rm -f *.txt
