#include "ll.h"
#include "account.h"

#ifndef ___bank_h___
#define ___bank_h___

#define MAX_ACCOUNTS 100												//Anzahl an maximal zur verfügungstehenden Konten

typedef struct {														//Hilsfstruktur für die Menüfunktion
	const char *desc;													//Beschreibung der Funktion
	int (*func)(ll_t **bank);											//Funktionspointer zur Funktion
} bank_action_t;

ll_t *bank_init(void);													//Erstellt eine neue Bank
int bank_create_account(ll_t **bank);									//Fügt der Bank einen neues Konto hinzu
int bank_delete_account(ll_t **bank);									//Löscht ein vorhandenes Konto
int bank_print_accounts(ll_t **bank);									//Gibt alle Konten aus
int bank_make_transaction(ll_t **bank);									//Führt eine Überweisung oder Ein-/Auszahlung durch
int bank_interest_distribution(ll_t **bank);							//Führt die Zinsberechnung durch
void bank_menu(ll_t **bank);											//Hauptmenü der Bank

#endif
