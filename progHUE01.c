#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "progHUE01.h"

char eingabe[500];

typedef struct {
	char *programmname;
	char *argumente[19];
} Befehl;

Befehl befehle[10];

// am Wochenende:
// Kontrolle kritischer Eingaben, zB a b; ; c
// eventuell Optimierung von strtok

// Leereichen am Ende des Strings, welches durch die Eingabe-Taste geschrieben
// wird nach der Eingabe, aus der Eingabe (eingabe) entfernen

// Überprüfen von i, j und k in den Schleifen

int main() {
	while(1) {

		printf("> ");

		if(fgets(eingabe, sizeof(eingabe), stdin) == NULL) {exit(0);}


		char *tempbefehle[10];
		char delimiter[] = ";";
		int i = 0;

		tempbefehle[i] = strtok(eingabe, delimiter);

		while(tempbefehle[i] != NULL && i < 10) {
			i++;
			tempbefehle[i] = strtok(NULL, delimiter);
		}

		for (int j = 0; j < i; j++) {
			char delimiterTwo[] = " ";
			char *temp;
			int k = 0;

			temp = strtok(tempbefehle[j], delimiterTwo);
			befehle[j].programmname = temp;

			while(temp != NULL && k < 19) {
				temp = strtok(NULL, delimiterTwo);
				befehle[j].argumente[k] = temp;
				k++;
			}
		}

		// schreibt die befehle auf der Konsole
		test();
	}

	return 0;
}

void test() {
	for (int i=0; i < 10; i++) {
		if (befehle[i].programmname != NULL) {printf("%s\n", befehle[i].programmname);}

		for (int j=0; j < 19; j++) {
			if (befehle[i].argumente[j] != NULL) {printf("%s\n", befehle[i].argumente[j]);}
		}
	}
}

/*
		Beachte!

		2) Die Eingabezeile besteht aus beliebig vielen "Befehlen" (maximal 10,
		moeglicherweise auch keinem), die jeweils durch ein Semikolon voneinander getrennt sind.

		3) Ein Befehl besteht aus "Worten" (maximal 20, moeglicherweise auch keinem):
		ein Programmname, gefolgt von bis zu 19 Argumenten, die jeweils durch ein oder
		mehrere Leerzeichen voneinander getrennt sind.
*/
