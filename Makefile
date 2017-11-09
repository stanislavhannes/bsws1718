CFLAGS= -m32 -g -Wall -ansi -pedantic -std=c99
CC = gcc

all: progHUE01

progHUE01: 	progHUE01.c
			$(CC) $(CFLAGS) -o $@ $^ 

%.o: 		%.c
			$(CC) $(CFLAGS) -o $@ -c $<

depend.mak: progHUE01.c
			gcc -MM $^ > $@  #Schreibt Lokale abhaengigkeiten in depend.mak

-include depend.mak  

dist-clean: clean
			rm -f progHUE01 depend.mak

clean :		
			rm -f *.o