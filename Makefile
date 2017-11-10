CFLAGS= -m32 -g -Wall -ansi -pedantic -std=c99
CC = gcc

all: 		readAndAnalyze

readAndAnalyze: 	readAndAnalyze.c
			$(CC) $(CFLAGS) -o $@ $^ 

%.o: 		%.c
			$(CC) $(CFLAGS) -o $@ -c $<

depend.mak: readAndAnalyze.c
			gcc -MM $^ > $@  #Schreibt Lokale abhaengigkeiten in depend.mak

-include depend.mak  

dist-clean: clean
			rm -f readAndAnalyze depend.mak

clean :		
			rm -f *.o