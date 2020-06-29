#include <stdint.h>

#ifndef ___ll_h___
#define ___ll_h___

typedef struct ll_s {													//Strukturtype für Verkettete Liste(LinkedList)
	void *data; 														// void-Pointer, für Daten
	struct ll_s *next; 													// Zeiger auf nächstes Element der Liste
} ll_t;

ll_t* ll_new(void *data);												//Erstellt neue Liste
void ll_append(ll_t *ll, void *data);									//Fügt einer Liste ein Element hinzu
ll_t* ll_get(ll_t *ll, size_t index);									//Gibt das Element am Index
void ll_remove(ll_t **p_ll, size_t index);								//Löscht das Element am Index
size_t ll_count(const ll_t *ll);										//Git Länge der Liste

ll_t* ll_search(ll_t *ll, int(*searchfn)(const void *, const void *), const void *search, int *idx);	//generische Suche
void ll_print(const ll_t *ll, void(*print_fn)(const void *p));			//generische Ausgabe

#endif
