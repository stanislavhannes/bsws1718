CFLAGS= -g -Wall -ansi -pedantic -std=c99
CC = gcc
CSOURCES= readAndAnalyze.c makeProcess.c

all: 		hu1

hu1: 		$(CSOURCES)
			$(CC) $(CFLAGS) -o $@ $^

%.o: 		%.c
			$(CC) $(CFLAGS) -o $@ -c $<

depend.mak: $(CSOURCES)
			gcc -MM $^ > $@  #Schreibt Lokale abhaengigkeiten in depend.mak

-include depend.mak  

dist-clean: clean
			rm -f hu1 depend.mak

clean :		
			rm -f *.o