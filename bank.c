#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "ll.h"
#include "account.h"
#include "bank.h"

#define ARR_LEN(arr) (sizeof(arr)/sizeof(arr[0]))						//Makro zur bestimmung von Array Größen

int getSicheresInt(int *zahl);											//Hilfsfunktion zum sicheren Einlesen von Integer
int getSicheresDouble(double *zahl);									//Hilfsfunktion zum sicheren Einlesen von Double
int eingabeRangeInt(int *zahl, const char* message, const char* errorMessage, const int begin, const int end);	//Eingabe von Integer in bestimmten Bereich
int eingabeMinDouble(double *zahl, const char* message, const char* errorMessage, const double begin);	//Eingabe von Double mit Mindestwert
int eingabeDouble(double *zahl, const char* message, const char* errorMessage);	//Eingabe von Double
//int eingabeMinInt(int *zahl, const char* message, const char* errorMessage, const int begin);	//Eingabe von Integer mit Mindestwert
int eingabeFindKontoNr(int *accountno, int *idx, ll_t *bank, const char* message, const char* errorMessage);	//Eingabe von vorhandener Kontonummer
int findFreeAccountno(ll_t *bank, int *accountno);						//Erzeugt eine neue Kontonummer und prüft auf nicht Vorhandensein

ll_t *bank_init(void) {													//Erstellt eine neue Bank
    account_checking_t *bank_konto = account_create_checking("Vereinigte Spassbanken", 0);	//Bankkonto anlegen
    ll_t *liste = ll_new(bank_konto);									//neue Kontoliste mit Bankkonto anlegen
    return liste;
} 

int bank_create_account(ll_t **bank) {									//Fügt der Bank einen neues Konto hinzu
	int neuekontonummer = 0;
	if(findFreeAccountno(*bank, &neuekontonummer) != 0) {				//Wenn keine neue Kontonummer verfügbar ist, dann Abbrechen mit Fehlermeldung
		printf("Die Bank kann nur %d Konten verwalten, Abbruch.\n", MAX_ACCOUNTS);
		return 1;
	}
	int eingabe_int = -1;												//Zuerst Konto Art wählen
	if(eingabeRangeInt(&eingabe_int, "Art des Kontos (0: Girokonto, 1: Sparbuch, 2: Kredit): ", "Bitte eine gueltige Kontoart waehlen.", 0 , 2) == 0) {
		int konto_typ = eingabe_int;
		char eingabe_name[ACCOUNT_NAME_LEN+2];
		do {
			printf("Name des Kontoinhabers: ");							//Konto Name eingeben
			fgets(eingabe_name, ACCOUNT_NAME_LEN+2, stdin);
			eingabe_name[strlen(eingabe_name)-1] = '\0';				//'\n' von String entfernen
			if(strlen(eingabe_name) > 0) {								//Überprüfen ob mehr als nichts eingegeben ist
				if(ll_search(*bank, account_search_name, eingabe_name, &eingabe_int) == NULL) {	//Name des Konto darf nicht vergeben sein
					break;
				}
			}
			printf("Auf Name existiert bereits ein Konto, oder leerer Name eingegeben.\n");	//Ansonsten Fehlermeldung ausgeben und Wiederholen
		} while(1);
		if(konto_typ > 0) {												//Bei Sparbuch und Kredit muss noch die Verzinsung eingegeben werden
			float verzinsung = 0.0f;
			double eingabe_double = 0.0;
			if(eingabeMinDouble(&eingabe_double, "Verzinsung in Prozent: ", "Bitte eine gueltige Verzinsung eingeben.", 0) == 0) {
				verzinsung = (float)eingabe_double;
				eingabe_double = 0.0;
				if(konto_typ > 1) {										//Bei Kredit muss noch der Kreditbetrag eingeben werden
					if(eingabeMinDouble(&eingabe_double, "Hoehe des Kredits: ", "Bitte eine gueltige Kredithoehe eingeben.", 0.0) == 0) {
						ll_append(*bank, account_create_loan(eingabe_name, neuekontonummer, verzinsung, (float)eingabe_double)); //Abschließend nooch den Kredit anlegen
						printf("Konto mit der Kontonummer %d angelegt.\n", neuekontonummer);	//Kontonummer noch ausgeben
						return 1;
					} else {
						printf("Eingabe Abgebrochen\n");				//Fehlermeldung bei Eingabe Abgebrochen
					}
				} else {
					ll_append(*bank, account_create_saving(eingabe_name, neuekontonummer, verzinsung));	//Abschließend noch das Sparbuch anlegen
					printf("Konto mit der Kontonummer %d angelegt.\n", neuekontonummer);	//Kontonummer noch ausgeben
					return 1;
				}
			} else {
				printf("Eingabe Abgebrochen\n");						//Fehlermeldung bei Eingabe Abgebrochen
			}
		} else {
			ll_append(*bank, account_create_checking(eingabe_name, neuekontonummer));	//Abschließend noch das Griokonto anlegen
			printf("Konto mit der Kontonummer %d angelegt.\n", neuekontonummer);	//Kontonummer noch ausgeben
			return 1;
		}
	} else {
		printf("Eingabe Abgebrochen\n");								//Fehlermeldung bei Eingabe Abgebrochen
	}
	printf("error at bank_create_account\n");							//Allgemeine Fehlermeldung beim Erstellen
	return -1;
} 

int bank_delete_account(ll_t **bank) {									//Löscht ein vorhandenes Konto
	if(ll_count(*bank) == 1) {											//Wenn kein Konto außer der Bank verfüabr ist,
		printf("Es gibt kein Konto zu loeschen!\n");					//dann mit Fehlermeldung abbrechen.
		return 1;
	}
	int accountno = 0, idx  = 0;										//Gültiges Konto eingeben
	if(eingabeFindKontoNr(&accountno, &idx, *bank, "Kontonummer des zu loeschenden Kontos: ", "Bitte eine gueltige Kontonummer eingeben. (0 ist Bank und damit ungueltig!)") == 0) {
		ll_remove(bank, idx);											//und Konto löschen
		return 1;
	}
	printf("error at bank_delete_account\n");							//Allgemeine Fehlermeldung beim Löschen
	return -1;
}

int bank_print_accounts(ll_t **bank) {									//Gibt alle Konten aus
    ll_print(*bank, account_print);										//generische Ausgabe aller Konten
    return 0;
}  

int bank_make_transaction(ll_t **bank) {								//Führt eine Überweisung oder Ein-/Auszahlung durch
	int eingabe_int = 0;												//Transaktionsart auswählen
	if(eingabeRangeInt(&eingabe_int, "Ueberweisung (0) oder Ein-/Auszahlung (1): ", "Bitte eine gueltige Transaktionsart waehlen.", 0, 1) == 0) {
		if(eingabe_int == 0) {											//Bei Überweisung
			int accountfrom = 0, idxfrom = 0, accountto = 0, idxto = 0;	//Gültige Konten eingeben
			if(eingabeFindKontoNr(&accountfrom, &idxfrom, *bank, "Ueberweisung von Konto: ", "Bitte eine gueltige Kontonummer eingeben. (0 ist Bank und damit ungueltig!)") == 0) {
				if(eingabeFindKontoNr(&accountto, &idxto, *bank, "Ueberweisung zu Konto: ", "Bitte eine gueltige Kontonummer eingeben. (0 ist Bank und damit ungueltig!)") == 0) {
					double eingabe_double = 0.0;						//Betrag eingeben
					if(eingabeDouble(&eingabe_double, "Betrag der Ueberweisung: ", "Bitte einen gueltigen Betrag eingeben.") == 0) {
						account_basic_t *fromkonto = (account_basic_t*)ll_get(*bank, idxfrom)->data;
						account_basic_t *tokonto = (account_basic_t*)ll_get(*bank, idxto)->data;
						account_add_transaction(fromkonto, accountto, (float)-eingabe_double);	//Überweisung ausführen
						account_add_transaction(tokonto, accountfrom, (float)eingabe_double);
						return 1;
					} else {
						printf("Eingabe  Abgebrochen\n");				//Fehlermeldung bei Eingabe Abgebrochen
					}
				} else {
					printf("Eingabe Abgebrochen\n");					//Fehlermeldung bei Eingabe Abgebrochen
				}
			} else {
				printf("Eingabe Abgebrochen\n");						//Fehlermeldung bei Eingabe Abgebrochen
			}
		} else if(eingabe_int == 1) {									//Bei Ein-/Auszahlung
			int accountno = 0, idx = 0;									//Gültige Konten eingeben
			if(eingabeFindKontoNr(&accountno, &idx, *bank, "Ein-/Auszahlung auf Konto: ", "Bitte eine gueltige Kontonummer eingeben. (0 ist Bank und damit ungueltig!)") == 0) {
				double eingabe_double = 0.0;							//Betrag eingeben
				if(eingabeDouble(&eingabe_double, "Betrag der Ein-/Auszahlung: ", "Bitte einen gueltigen Betrag eingeben.") == 0) {
					account_basic_t *konto = (account_basic_t*)ll_get(*bank, idx)->data;
					account_add_transaction(konto, -1, (float)eingabe_double);	//Einzahlung vornehmen
					return 1;
				} else {
					printf("Eingabe Abgebrochen\n");					//Fehlermeldung bei Eingabe Abgebrochen
				}
			} else {
				printf("Eingabe Abgebrochen\n");						//Fehlermeldung bei Eingabe Abgebrochen
			}
		}
	} else {
		printf("Eingabe Abgebrochen\n");								//Fehlermeldung bei Eingabe Abgebrochen
	}		
	printf("error at bank_make_transaction\n");							//Allgemeiner Fehler bei Transaktion
	return -1;
}  

int bank_interest_distribution(ll_t **bank) {							//Führt die Zinsberechnung durch
	account_basic_t *bankkonto = (account_basic_t*)ll_get(*bank, 0)->data;	//Bankkonto auswählen
	ll_t * ll =  (*bank)->next;											//Erstes Arbeitskonto wählen
	while(ll != NULL) {													//Durch alle Konten Iterieren
		account_basic_t *konto = (account_basic_t*)ll->data;
		float interest = account_add_interest(konto, bankkonto->accountno);	//Zinsen berechnen
		account_add_transaction(bankkonto, konto->accountno, -interest);	//der Bank Zinsen abziehen/gutschreiben
		ll = ll->next;													//nächstes Element auswählen
	}
	return 0;
}

void bank_menu(ll_t **bank) {											//Hauptmenü der Bank
	bank_action_t funktionen[] = {{"Neues Konto anlegen", bank_create_account},	//Alle Funktionen der bank, die sie Unterstützt.
								{"Existierendes Konto loeschen", bank_delete_account},
								{"Alle Konten ausgeben", bank_print_accounts},
								{"Transaktion eingeben", bank_make_transaction},
								{"Zinsausschuettung durchfuehren", bank_interest_distribution},
								{"Bank verlassen/Beenden", NULL}};
		
	printf("Herzlich Willkommen bei den Vereinigten Spassbanken.\n(Eingaben mit 'q' abbrechen.)\n");
	int i, eingabe;
	while(1) {															//Hauptprogrammschleife
		i = 0;
		while( i < (int)ARR_LEN(funktionen)) {							//Alle Funktionen auflisten
			printf("\t(%d) %s\n", i, funktionen[i].desc);
			i++;
		}
		eingabe = -1;													//Eingabe einer gültigen Nummer für eine Funktion
		if(eingabeRangeInt(&eingabe, "Aktion waehlen: ", "Bitte eine gueltige Aktion waehlen.", 0, 5) == 0) {
			if(eingabe == 5) {											//Bei Funktion 5,
				break;													//dass Programm beenden
			}
			printf("\n");												//Schönheitskorrektur
			i = funktionen[eingabe].func(bank);							//Funktion ausführen
			if(i == 1) {												//Schönheitskorrektur
				printf("\n");
			}
		} else {
			printf("Eingabe Abgebrochen\n");							//Fehlermeldung bei Eingabe Abgebrochen
		}
	};
	printf("Danke fuer Ihren Besuch.\n");
	return;
}

///////////////////////////////////////////////////////////////////////////Hilfsfunktionen//////////////////////////////////////////////////////////////////////

int getSicheresInt(int *zahl) {											//Hilfsfunktion zum sicheren Einlesen von Integer
	int minus = 0;
	char eingabe[101];
	fgets(eingabe, 100, stdin);											//Zahl einlesen
	*zahl = 0;
	int cnt = 0, len = strlen(eingabe)+1;
	if(len <= 2) {														//Sofern Eingabelänge unterschreitet,
		return -1;														//dann eingabe Abbrechen
	}
	while(cnt  < len) { //Zahlen										//Jedes Zeichen auf Gültigkeit prüfen und verarbeiten
		//printf("C: [%d]=%d\t",cnt,  *(eingabe+cnt));
		if(*(eingabe+cnt) >= '0' && *(eingabe+cnt) <= '9') { //Zahlen
			*zahl *= 10;
			*zahl += *(eingabe+cnt)-'0';
		} else 
		if(*(eingabe+cnt) == ' ') {} else //Leerzeichen
		if(*(eingabe+cnt) == '-') { //Minus
			minus++;
		} else
		if(*(eingabe+cnt) == 'q') { //Eingabe Abbrechen
			return -4;
		} else
		if(*(eingabe+cnt) == '\n') { //Enter
			*zahl *= minus%2 == 1 ? -1 : 1;
			return 0;
		} else {
			break;
		}				
		cnt++;
	}
	return -1;
}

double pow10m(int exp) {												//Hilfsfunktion zum Potenzieren von 10
	double z = 1;
	while(exp > 0) {
		z *= 10.0;
		exp--;
	}
	return z;
}
int getSicheresDouble(double *zahl) {									//Hilfsfunktion zum sicheren Einlesen von Double
	int minus = 0;
	int dot = 0;
	char eingabe[101];
	fgets(eingabe, 100, stdin);											//Zahl einlesen
	*zahl = 0;
	int cnt = 0, len = strlen(eingabe)+1;
	if(len <= 2) {														//Sofern Eingabelänge unterschreitet,
		return -1;														//dann eingabe Abbrechen
	}
	while(cnt  < len) { //Zahlen										//Jedes Zeichen auf Gültigkeit prüfen und verarbeiten
		//printf("C: [%d]=%d\t",cnt,  *(eingabe+cnt));
		if(*(eingabe+cnt) >= '0' && *(eingabe+cnt) <= '9') { //Zahlen
			if(dot == 0) {
				*zahl *= 10;
				*zahl += *(eingabe+cnt)-'0';
			} else {	//Nachkommazahlen
				double z = *(eingabe+cnt)-'0';
				*zahl += z / (pow10m(dot++));
				//printf("z=%f, *zahl=%f\t", z, *zahl);
			}
		} else 
		if(*(eingabe+cnt) == ' ') {} else //Leerzeichen
		if(*(eingabe+cnt) == '-') { //Minus
			minus++;
		} else
		if(*(eingabe+cnt) == '.') { //Komma
			if(dot > 0) {
				return -3; // Zwei Kommas
			}
			dot = 1;
		} else
		if(*(eingabe+cnt) == 'q') { //Eingabe Abbrechen
			return -4;
		} else
		if(*(eingabe+cnt) == '\n' || *(eingabe+cnt) == ',') { //Enter
			*zahl *= minus%2 == 1 ? -1 : 1;
			return 0;
		} else {
			return -2; //Ungültige Eingabe
		}				
		cnt++;
	}
	return -1;
}

int eingabeRangeInt(int *zahl, const char* message, const char* errorMessage, const int begin, const int end) {	//Eingabe von Integer in bestimmten Bereich
	do {
		printf("%s", message);											//Eingabeaufforderung anzeigen
		int ret = getSicheresInt(zahl);									//Zahl eingeben
		if(ret == 0) { // Fehlerfreie Eingabe
			if(*zahl >= begin && *zahl <= end) {						//Wenn Zahl im Bereich liegt,
				return 0;												//dann beenden
			}
		} else 
		if(ret == -4) { 												//Bei Eingabe abgebrochen,
			return -4;													//dann beenden
		}
		printf("%s\n", errorMessage);									//Sonst Fehlermeldung ausgeben
	} while(1) ;
	return -1;
}

int eingabeMinDouble(double *zahl, const char* message, const char* errorMessage, const double begin) {
	do {
		printf("%s", message);											//Eingabeaufforderung anzeigen
		int ret = getSicheresDouble(zahl);								//Zahl eingeben
		if(ret == 0) { // Fehlerfreie Eingabe
			if(*zahl >= begin) {										//Wenn Zahl über Minimum liegt,
				return 0;												//dann beenden
			}
		} else 
		if(ret == -4) { 												//Bei Eingabe abgebrochen,
			return -4;													//dann beenden
		}
		printf("%s\n", errorMessage);									//Sonst Fehlermeldung ausgeben
	} while(1) ;
	return -1;
}

int eingabeDouble(double *zahl, const char* message, const char* errorMessage) {
	do {
		printf("%s", message);											//Eingabeaufforderung anzeigen
		int ret = getSicheresDouble(zahl);								//Zahl eingeben
		if(ret == 0) { // Fehlerfreie Eingabe
			return 0;													//beenden
		} else 
		if(ret == -4) { 												//Bei Eingabe abgebrochen,
			return -4;													//dann beenden
		}
		printf("%s\n", errorMessage);									//Sonst Fehlermeldung ausgeben
	} while(1) ;
	return -1;
}

/*
int eingabeMinInt(int *zahl, const char* message, const char* errorMessage, const int begin) {	//Eingabe von Integer mit Mindestwert
	do {
		printf("%s", message);											//Eingabeaufforderung anzeigen
		int ret = getSicheresInt(zahl);									//Zahl eingeben
		if(ret == 0) { // Fehlerfreie Eingabe
			if(*zahl >= begin) {{										//Wenn Zahl über Minimum liegt,
				return 0;												//dann beenden
			}
		} else 
		if(ret == -4) { 												//Bei Eingabe abgebrochen,
			return -4;													//dann beenden
		}
		printf("%s\n", errorMessage);									//Sonst Fehlermeldung ausgeben
	} while(1) ;
	return -1;
}
*/

int eingabeFindKontoNr(int *accountno, int *idx, ll_t *bank, const char* message, const char* errorMessage) {
	do {
		printf("%s", message);											//Eingabeaufforderung anzeigen
		*accountno = 0;
		int ret = getSicheresInt(accountno);							//Kontonummer eingeben
		if(ret == 0) { // Fehlerfreie Eingabe
			if(*accountno > 0) {
				if(ll_search(bank, account_search_accountno, accountno, idx) != NULL) { //Wenn Konto existiert und nicht Bank ist,
					return 0;											//dann beenden
				}
			}
		} else 
		if(ret == -4) { 												//Bei Eingabe abgebrochen,
			return -4;													//dann beenden
		}
		printf("%s\n", errorMessage);									//Sonst Fehlermeldung ausgeben
	} while(1) ;
	return -1;
}

int findFreeAccountno(ll_t *bank, int *accountno) {
	ll_t *nochfrei;
	int erste = 1;
	int i = 1, idx = 0;
	int zahlen[MAX_ACCOUNTS];
	for(int k=0; k<MAX_ACCOUNTS; k++) { zahlen[k] = k; }				//Hilfslist mit allen Zahlen anlegen
	
	while(i < MAX_ACCOUNTS) {
		if(ll_search(bank, account_search_accountno, zahlen+i, &idx) == NULL) {	//Wenn Kontonummer nicht existiert in "nochfrei"-Liste eintragen
			if(erste == 1) {											//Liste neu anlegen oder hinzufügen
				nochfrei = ll_new(zahlen+i);
				erste = 0;
			} else {
				ll_append(nochfrei, zahlen+i);
			}
		}
		i++;															//Index erhöhen um nächste Kontonummer zu prüfen
	}
	int count = ll_count(nochfrei);										//Anzahl der freien Kontonummern
	if(count > 0) {
		*accountno = *((int*)ll_get(nochfrei, rand()%count)->data);		//Zufällige Kontonummer auswählen und abspeichern
		return 0;														//erfolgreich beenden
	}
	return -1;															//Wenn keine Konten mehr frei sind, Fehler zurückgeben
}
