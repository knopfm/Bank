#ifndef ___account_h___
#define ___account_h___


#define ACCOUNT_NAME_LEN 30												//Maximallänge für Kontoinhaber Name
typedef enum {ACCOUNT_CHECKING=0, ACCOUNT_SAVING, ACCOUNT_LOAN} account_type_t;	//enum für Kontotypen

#define ACCOUNT_BASIC account_type_t type; char name[ACCOUNT_NAME_LEN+1]; int accountno; float balance;	//Elemente des Basis Accounts

typedef struct 
{
	ACCOUNT_BASIC
} account_basic_t; //Basis Account (abstract)

typedef struct 
{
	int accountno;
	float amount;
} transaction_t; //Basis Transaktion (abstract)

#define ACCOUNT_TRANSACTION transaction_t *transactions; int transactions_curr; int transactions_cap;	//Elemente einer Transaktion

typedef struct
{
	ACCOUNT_BASIC
	ACCOUNT_TRANSACTION
} account_transaction_t; //BasisTransaktions Account : Basis, Transaktion 	(Vererbung)

typedef account_transaction_t account_checking_t; //Griokonto : Basis, Transaktion 	(Vererbung)

typedef struct
{
	ACCOUNT_BASIC
	ACCOUNT_TRANSACTION
	float interest;
} account_saving_t; //Sparbuch : Basis, Transaktion	(Vererbung)

typedef struct
{
	ACCOUNT_BASIC
	float interest;
	float total_loan;
}account_loan_t; //Kredit : Basis	(Vererbung)

account_checking_t* account_create_checking(const char *name, int accountno);	//Legt ein neues Girokonto an
account_saving_t* account_create_saving(const char *name, int accountno, float interest);	//Legt ein neues Sparbuch an
account_loan_t* account_create_loan(const char *name, int accountno, float interest, float total_loan);	//legt einen neuen Kredit an
//static void print_transactions(account_basic_t *ac);					//Transaktionen ausgeben
void account_print(const void *ac);										//Gibt ein Konten aus, Hilfsfunktion für ll_print
void account_add_transaction(account_basic_t *ac, int accountno, float amount);	//Führt eine Überweisung druch
int account_search_accountno(const void *ac, const void *no);			//Hilfsfunktion für ll_search zum Suchen von Kontonummern
int account_search_name(const void *ac, const void *name);				//Hilfsfunktion für ll_search zum Suchen von Kontoinhabern
float account_add_interest(account_basic_t *ac, int accountnobank);		//Führt die Zinsberechnung durch

#endif
