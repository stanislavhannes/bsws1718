#include<stdio.h>
#include<stdlib.h>

#include "readAndAnalyze.h"

char *input = NULL; /* eingabe */
size_t length;
int inputLength;

char commands[10][20]; /* befehle */

int main() {
	while(1) {

		printf("> ");

	  inputLength = getline(&input, &length, stdin);

		if (inputLength == 1) {
			continue;
		}
		else if (inputLength > 501) {
			printf("Ihr input ist zu groß\n");
			continue;
		}

		/* Zerlegen des char-Array's in einzelne Befehle getrennt durch ein Semikolon
		 Aufrufen einer Funktion, die das erledigt */
		splitInput();

		printf("%s", input);
		printf("%d\n", inputLength);
		inputLength = 0;
	}

	return 0;
}

/*
	Vorbereitung der Aufteilung der einzelnen Befehle:
	1) mehrere aufeinander folgende leere Befehle aussortieren
	? 2) mehrere aufeinander folgende Leerzeichen aussortieren ?
*/

/*
	Beachte!
	2) Die inputzeile besteht aus beliebig vielen "Befehlen" (maximal 10,
	moeglicherweise auch keinem), die jeweils durch ein Semikolon voneinander getrennt sind.

	3) Ein Befehl besteht aus "Worten" (maximal 20, moeglicherweise auch keinem):
	ein Programmname, gefolgt von bis zu 19 Argumenten, die jeweils durch ein oder
	mehrere Leerzeichen voneinander getrennt sind.
*/
void splitInput() { /*splitEingabe*/
	for(int i=0; i < inputLength-1; i++){
		if(input[i] == 59) {
			// BSP: "hallo;"
			printf("%c in split gefunden\n", input[i]);

			// alles was vor der input i steht wird an die Stelle 0 gepackt
			// weiche um 1 erhöhen
			// weiter bis zum nächsten, vorherige Stelle in einer Variablen merken
		}
	}
}
