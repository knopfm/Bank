#include <stdint.h>
#include <malloc.h>
#include "ll.h"


ll_t* ll_new(void *data) {												//Legt eine neue Verknüpfte Liste (LinkedList) an
	ll_t*  element = (ll_t*)malloc(sizeof(ll_t)); 						//Speicher Reservieren für ein Element
	if(element != NULL) {												//Wenn Speicher reserviert, dann
		element->data = data;											//Element initalisieren und Daten abspeichern
		element->next = NULL;
	}
	return element;														//Neue Liste zurückgeben
}

ll_t* ll_getLast(ll_t *ll) {											//Gibt das Letzte Element der Liste zurück
	if(ll != NULL) {													//Wenn Liste vorhanden, dann bis zum Ende iterieren und
		ll_t* current = ll;
		while(current->next != NULL) {
			current = current->next;
		}
		return current;													//das letzte Element zurück geben
	}
	return NULL;
}

void ll_append(ll_t *ll, void *data){									//Fügt der vorhanden Liste ein neues Element hinzu
	while(ll->next != NULL) {											//Bis zum Ende iterieren und
        ll = ll->next;
    }
    ll->next = ll_new(data); 											//dem letzten Element ein neues Element hinzufügen
}

ll_t* ll_get(ll_t *ll, size_t index) {									//Das Element an der Stelle des Index zurück geben
	size_t i = 0;
	while(ll != NULL) {
		if(i == index) {												//Vergleichen, ob aktuelles Element dem gesuchten Index entspricht
			return ll;													//bei Erfolg, das aktuelle Element zurück geben
		}
		ll = ll->next;
		i++;															//Hilfsvaribale als Index zum Suchen erhöhen
	}
	return ll;
}

void ll_remove(ll_t **p_ll, size_t index) {								//Das Element an der Stelle des Index löschen
	if(index == 0) {													//Unterscheiden ob das erste Element gelöscht werden muss
        ll_t *next = (*p_ll)->next;										//Nächstes Element zwischenspeichern
        free((*p_ll)->data);											//Daten löschen
        free(*p_ll);													//Element löschen
        *p_ll = next;													//Zweites Element auf Erstes setzten
    }
    else {
        ll_t *prev = ll_get(*p_ll, index-1);							//Vorherriges Element raussuchen
        ll_t *to_delete = prev->next;									//Zu löschendes Element raussuchen
        if(to_delete != NULL) {											//Wenn es etwas zu löschen gibt
            prev->next = to_delete->next;								//dann nächstes Element bei Vorherigem als nächstes setzten
            free(to_delete->data);										//Daten löschen
            free(to_delete);											//Element löschen
        }
    }
}

size_t ll_count(const ll_t *ll) {										//Gibt die Anzhal der Elemente der Liste zurück
	size_t cnt = 0;														//Hilfvaribale zum zählen
	while(ll != NULL) {													//Bis zum Ende iterieren und Counter hochzählen
		ll = ll->next;
		cnt++;
	}
	return cnt;
}

ll_t* ll_search(ll_t *ll, int(*searchfn)(const void *, const void *), const void *search, int *idx) {	//Funktion zum generischen Suchen von Daten
	*idx = 0;
	while(ll != NULL) {													//Durch Liste iterieren und Suchfunktion aufrufen, um Daten zu finden
		if(searchfn(ll->data, search) == 1) { 							//Daten und Suchwort als void* -Pointer
			return ll;													//Wenn Daten gefunden wurden, dann Element zurück geben
		}
		*idx += 1;														//Hilfsindex für Suche in der Liste erhöhen
		ll = ll->next;													//Durch Liste iterieren
	}
	*idx = -1;
	return NULL;														//Bei nicht auffinden des Elements, Index auf -1 und kein Element zurück geben
}

void ll_print(const ll_t *ll, void(*print_fn)(const void *)) {			//Generische Ausgabe von Daten der Liste
	while(ll != NULL) {													//Durch die Liste iterieren und
		print_fn(ll->data);												//Daten mit eigener Funktion ausgeben
		ll = ll->next;
	}
}
