#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <malloc.h>
#include "account.h"

account_checking_t* account_create_checking(const char *name, int accountno) {	//Neues Griokonto anlegen
	account_checking_t *new_acc = (account_checking_t*)malloc(sizeof(account_checking_t));	//Speicherplatz reservieren
	new_acc->type = ACCOUNT_CHECKING;									//Den Typ festlegen,
	new_acc->accountno = accountno;										//Kontonummer festlegen,
	strncpy(new_acc->name, name, ACCOUNT_NAME_LEN+1);					//Name eintragen,
	new_acc->transactions_curr = 0;										//Transaktionstabell initialisieren,
	new_acc->transactions_cap = 0;	
	new_acc->transactions = NULL;
	new_acc->balance = 0.0f;											//Kontostand zurücksetzten
	return new_acc;														//und Kontoadresse zurückgeben.
}

account_saving_t* account_create_saving(const char *name, int accountno, float interest) {	//Neues Sparbuch anlegen
	account_saving_t *new_acc = (account_saving_t*)malloc(sizeof(account_saving_t));	//Speicherplatz reservieren
	new_acc->type = ACCOUNT_SAVING;										//Den Typ festlegen,
	new_acc->accountno = accountno;										//Kontonummer festlegen,
	strncpy(new_acc->name, name, ACCOUNT_NAME_LEN+1);					//Name eintragen,
	new_acc->interest = interest;										//Zinsatz festlegen,
	new_acc->transactions_curr = 0;										//Transaktionstabell initialisieren,
	new_acc->transactions_cap = 0;	
	new_acc->transactions = NULL;
	new_acc->balance = 0.0f;											//Kontostand zurücksetzten
	return new_acc;														//und Kontoadresse zurückgeben.
}

account_loan_t* account_create_loan(const char *name, int accountno, float interest, float total_loan) {	//Neuer Kredit anlegen
	account_loan_t *new_acc = (account_loan_t*)malloc(sizeof(account_loan_t));	//Speicherplatz reservieren
	new_acc->type = ACCOUNT_LOAN;										//Den Typ festlegen,
	new_acc->accountno = accountno;										//Kontonummer festlegen,
	strncpy(new_acc->name, name, ACCOUNT_NAME_LEN+1);					//Name eintragen,
	new_acc->interest = interest;										//Zinsatz festlegen,
	new_acc->total_loan = total_loan;									//Kredithöhe eintragen,
	new_acc->balance = -total_loan;										//Kontostand einstellen
	return new_acc;														//und Kontoadresse zurückgeben.
}

static void print_transactions(account_basic_t *ac) {					//Transaktionen ausgeben
	if(ac->type == ACCOUNT_CHECKING || ac->type == ACCOUNT_SAVING) {	//Nur beim Girokonto und Sparbuch anzeigen
		account_transaction_t *ac_t = (account_transaction_t*)ac;
		int i = 0;
		while(i < ac_t->transactions_curr) {							//Schleife durch alle Transaktionen
			if(ac_t->transactions[i].accountno == -1) {					//-1 für Ein-/Auszahlung
				printf(" Ein-/Ausz. | %.2f\n", ac_t->transactions[i].amount);
			} else {													//oder Kontonummer für Überweisung asugeben
				printf("%11d | %.2f\n", ac_t->transactions[i].accountno, ac_t->transactions[i].amount);
			}
			i++;
		}
	}
}

void account_print(const void *ac) {									//Konto ausgeben, Hilfsfunktion für ll_print um alle Konten auszugeben
	account_basic_t *ac_t = (account_basic_t*)ac;
	if(ac_t->type == ACCOUNT_CHECKING) {								//Entsprechend den Typ ausgeben
	printf("Kontotyp:       Girokonto\n");
	} else if(ac_t->type == ACCOUNT_SAVING) {
	printf("Kontotyp:       Sparbuch\n");
	} else if(ac_t->type == ACCOUNT_LOAN) {
	printf("Kontotyp:       Kredit\n");
	}
	printf("Kontoinhaber:   %s\n", ac_t->name);							//Allgemeine Kontoinformationen ausgeben
	printf("Kontonummer:    %d\n", ac_t->accountno);
	printf("Guthaben:       %.2f\n", ac_t->balance);
	if(ac_t->type == ACCOUNT_SAVING) {									//Bei Griokonto zusätzlich Zinsen ausgeben
		account_saving_t *ac_s = (account_saving_t*)ac_t;
		printf("Zinsen:         %.2f\n", ac_s->interest);
	}
	if(ac_t->type == ACCOUNT_LOAN) {									//Bei Kredit zusätzlich Zinsen und Kredithöhe ausgeben
		account_loan_t *ac_l = (account_loan_t*)ac_t;
		printf("Zinsen:         %.2f\n", ac_l->interest);
		printf("Gesamtleihgabe: %.2f\n", ac_l->total_loan);
	}
	if(ac_t->type == ACCOUNT_CHECKING || ac_t->type == ACCOUNT_SAVING) {	//Bei Girokonto und Sparbuch noch die Transaktionen ausgeben
		printf("Kontonummer | Betrag\n");
		print_transactions(ac_t);
	}
	printf("\n");														//Schönheitskorrektur
}

void account_add_transaction(account_basic_t *ac, int accountno, float amount) {	//Einem Konto einen Betrag gutschreiben/abziehen;
	if(amount != 0.0f) {												//Wenn Betrag ungleich 0, dann verbuchen
		if(ac->type == ACCOUNT_CHECKING || ac->type == ACCOUNT_SAVING) {	//Bei Griokonto und Sprabuch werden diese gespeichert
			account_transaction_t *ac_t = (account_transaction_t*)ac;
			if(ac_t->transactions_curr == ac_t->transactions_cap) {		//Wenn Transaktionsspeicher zu klein ist, dann vergrößern
				ac_t->transactions_cap += 10;							//um 10 Elemente
				ac_t->transactions = (transaction_t*)realloc(ac_t->transactions, sizeof(transaction_t) * ac_t->transactions_cap);
				if(ac_t->transactions == NULL) {						//Wenn kein Speicher mehr verfügbar ist, dann Fehler ausgeben und abbrechen
					printf("error at realloc in add_transactions\n");
					return;
				}
			}
			ac_t->transactions[ac_t->transactions_curr].accountno = accountno;	//Dem letzten Element die Daten der Transaktion übergeben
			ac_t->transactions[ac_t->transactions_curr].amount = amount;
			ac_t->transactions_curr += 1;								//Index dex letzten Elements erhöhen
		}
		ac->balance += amount;											//Betrag auf dem  Konto gutschreiben/abziehen
	}
	return;
}

int account_search_accountno(const void *ac, const void *no) {			//Hilsfunktion für ll_search um Kontonummer zu suchen
	account_basic_t *left = (account_basic_t*)ac;
	int *right = (int*)no;
	return left->accountno == *right;
}

int account_search_name(const void *ac, const void *name) {				//Hilsfunktion für ll_search um Kontonamen zu suchen
	account_basic_t *left = (account_basic_t*)ac;
	char* right = (char*)name;
	return strncmp(left->name, right, ACCOUNT_NAME_LEN) == 0;			//Nur maximal Anzahl an Zeichen vergleichen, um Speicherüberschreitungen zu vermeiden
}

float account_add_interest(account_basic_t *ac, int accountnobank) {	//Dem Konto die Zinsen berechnen
	if(ac->type == ACCOUNT_SAVING) {									//Für das Girokonto die Zinsen berechen und den Betrag auf dem Konto überweisen
		account_saving_t *ac_s = (account_saving_t*)ac;
		float diff = (ac_s->interest / 100.0) * ac_s->balance;
		account_add_transaction((account_basic_t*)ac_s, accountnobank, diff);
		return diff;													//Zinsbetrag an die Bank zurückgeben
	}
	if(ac->type == ACCOUNT_LOAN) {										//Für Kredit die Zinsen berechnen
		account_loan_t *ac_l = (account_loan_t*)ac;
		float diff = (ac_l->interest / 100.0) * ac_l->balance;
		account_add_transaction((account_basic_t*)ac_l, accountnobank, diff);
		return 0.0f;													//Die Bank erhält keine Zinsen (wird nur auf dem Guthaben des Kredites aufgerechnet)
	}
	return 0.0f;
}
