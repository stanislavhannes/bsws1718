CFLAGS= -m32 -g -Wall -ansi -pedantic -std=c99
CC = gcc

all: progHUE01

progHUE01: 	progHUE01.c
			$(CC) $(CFLAGS) -o $@ $^ 

%.o: 		%.c
			$(CC) $(CFLAGS) -o $@ -c $<