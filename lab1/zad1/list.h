#ifndef LIST_H
#define LIST_H

#include <stdlib.h>

#include "contacts.h"
typedef struct ListNode ListNode;
struct ListNode{
	ListNode* next;
	ListNode* prev;
	Contact* data;
};

typedef struct List{
	ListNode* head;
	ListNode* tail;
	int len;
}List;


/*
- tworzenie i usuwanie ksiazki kontaktowej 
*/
ListNode* makeListNode(Contact* data);
void delListNode(ListNode* n);
List* makeList();
void delList(List* l);
/*
- dodanie i usuniecie  kontaktu do/z ksiazki (bez utraty innych kontaktow i bez wyciekow pamieci) 
*/
void listAddContact(List* l,Contact* c);
void listDelContact(List* l,Contact* c);
/*
- wyszukiwanie elementu w ksiazce
*/
ListNode* listFindContactNode(List* l,Contact* c);
Contact* listFindContactByKey(List *l,ContactKey key,char* val);
/*
- sortowanie/przebudowanie ksiazki wg wybranego pola (Nazwisko, Data urodzenia, email, telefon)
*/
void listMerge(ListNode* start,ListNode* mid,int l,int q, int r, ContactKey key);
void listSort(ListNode* start,int l,int r, ContactKey key);
void listSortFull(List* l,ContactKey key);
/* extra */
void listPrint(List* l);
#endif