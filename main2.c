//Matrikelnummer: xxxxxxx, TDM-GCC-64: 5.1.0 

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <malloc.h>

#include "ll.h"
#include "account.h"
#include "bank.h"

//#define DEBUG

int matrikelnr(char *nr, int maxlen) {									//Matrikelnummer zurückgeben, für die automatische Auswertung
	if(maxlen > (int) strlen("xxxxxxx")) {								//Wenn maxlen größergleich 8 Zeichen hat, dann wird die Matrikelnummer an den Zeiger *nr kopiert.
		strcpy(nr, "xxxxxxx");
		return 0;
	}
	return -1;
}

ll_t *konten;

int main(void) {														//Hauptprogramm
	konten = bank_init();												//neue Bank erstellen
	bank_menu(&konten);													//und Hauptmenü ausführen
    return 0;
}
