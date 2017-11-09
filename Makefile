
all: progHUE01

progHUE01: progHUE01.c
		gcc -m32 -g -Wall -ansi -pedantic -o $@ $^
