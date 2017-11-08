#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "progHUE01.h"

char *eingabe = NULL;
size_t length;
int inputLength;

struct befehl {
	char *programmname;
	char *argumente;
};

//struct befehl befehle[10];

int main() {
	while(1) {

		printf("> ");

	  inputLength = getline(&eingabe, &length, stdin);

		if (inputLength == 1) {continue;}
		else if (inputLength > 501) {
			printf("Ihr Eingabe ist zu groß\n");
			continue;
		}

		int correct = prepareEingabe();

		if (correct == -1) {
			printf("Zu viele Befehle!\n");
			continue;
		}

		if (eingabe[0] == ';' && eingabe[1] == '\n') {continue;}

		splitEingabe();

		printf("%s", eingabe);
		printf("%d\n", inputLength);
		inputLength = 0;
		eingabe = NULL;
	}

	return 0;
}

/*
	Beachte!

	2) Die Eingabezeile besteht aus beliebig vielen "Befehlen" (maximal 10,
	moeglicherweise auch keinem), die jeweils durch ein Semikolon voneinander getrennt sind.

	3) Ein Befehl besteht aus "Worten" (maximal 20, moeglicherweise auch keinem):
	ein Programmname, gefolgt von bis zu 19 Argumenten, die jeweils durch ein oder
	mehrere Leerzeichen voneinander getrennt sind.
*/
void splitEingabe() {
	/*//int weiche = 0;
	int grenze = 0;
	struct befehl temp;
	int countSpaces = 0;
	int j = 0;

	// erst die Befehle zählen, wenn gleich ein dann bis zum Semikolon,
	// ansonsten bis zum ersten Leerzeichen
	// => Leerzeichen zählen
	// 0 - nur progname
	// > 0 mit argumente

	for(int i=0; i < inputLength; i++){
		if(eingabe[i] == ' ') {countSpaces++;}

		if(eingabe[i] == ';') {

			if(countSpaces == 0) {
				while(eingabe[grenze] != ';') {
					temp.programmname[j++] = eingabe[grenze++];
					printf("yihaa\n");
				}
			}


			// BSP: ls -al; ps; hurz 42; grep hallo test.c
			// von grenze bis zum ersten leerzeichen


			// von grenze bis zu i-1

			// alles was vor der eingabe i steht wird an die Stelle 0 gepackt
			// weiche um 1 erhöhen
			// weiter bis zum nächsten, vorherige Stelle in einer Variablen merken
		}
	}*/
}

// Leerzeichen und leere Eingaben entfernen
// ; hinten anhängen und vorne entfernen
int prepareEingabe() {
	eingabe[inputLength-1] = ';';
	eingabe[inputLength] = '\n';

	for(int i=0; i < inputLength; i++){
		if(eingabe[i] == ' ' && eingabe[i+1] == ' ') {
			memmove(&eingabe[i], &eingabe[i + 1], strlen(eingabe) - i);
			i--;
			inputLength--;
		}
	}

	for(int i=0; i < inputLength; i++){
		if(eingabe[i-1] == ';' && eingabe[i] == ' ') {
			memmove(&eingabe[i], &eingabe[i + 1], strlen(eingabe) - i);
			i--;
			inputLength--;
		}
	}

	if(eingabe[0] == ' ') {
		memmove(&eingabe[0], &eingabe[0 + 1], strlen(eingabe) - 0);
		inputLength--;
	}

	for(int i=0; i < inputLength; i++){
		if(eingabe[i-1] == ';' && eingabe[i] == ';') {
			memmove(&eingabe[i], &eingabe[i + 1], strlen(eingabe) - i);
			i--;
			inputLength--;
		}
	}

	for(int i=0; i < inputLength; i++){
		if(eingabe[i] == ' ' && eingabe[i+1] == ';') {
			memmove(&eingabe[i], &eingabe[i + 1], strlen(eingabe) - i);
			i--;
			inputLength--;
		}
	}

	if(eingabe[0] == ';' && eingabe[1] != '\n') {
		memmove(&eingabe[0], &eingabe[0 + 1], strlen(eingabe) - 0);
	}

	int validiereEingabe = 0;
	for(int i=0; i < inputLength; i++){
		if(eingabe[i] == ';') {
			validiereEingabe++;
		}
	}
	if (validiereEingabe > 10) {
		return -1;
	}
	return 0;
}
