#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdio.h>

#include "contacts.h"
#include "list.h"

/*
- tworzenie i usuwanie ksiazki kontaktowej 
*/
ListNode* makeListNode(Contact* data){
	ListNode* n = malloc(sizeof(ListNode));
	n->next = NULL;
	n->prev = NULL;
	if(data==NULL){
		n->data=NULL;
	}
	else{
		n->data = makeContact();
		initContact(n->data,data->name,data->surName,data->birthDate,data->email,data->phone,data->address);
	}
	return n;
}
void delListNode(ListNode* n){
	assert(n!=NULL);
	if(n->data!=NULL){
		delContact(n->data);
	}
	free(n);
}
List* makeList(){
	List* l = malloc(sizeof(List));
	l->len=0;
	ListNode* guardianHead=makeListNode(NULL);/* head -> next to head prawdziwy */
	ListNode* guardianTail=makeListNode(NULL);/* tail -> prev to tail prawdziwy */
	guardianHead->next=guardianTail;
	guardianHead->prev=guardianTail;
	guardianTail->next=guardianHead;
	guardianTail->prev=guardianHead;
	l->head=guardianHead;
	l->tail=guardianTail;
	return l;
}
void delList(List* l){
	assert(l!=NULL);
	ListNode* p = l->head;
	ListNode* prev;
	for(int i=0;i<=l->len;i++){
		prev=p;
		p=p->next;
		delListNode(prev);
	}
	free(p);/* ostatni guardian */
	free(l);
}
/*
- dodanie i usuniecie  kontaktu do/z ksiazki (bez utraty innych kontaktow i bez wyciekow pamieci) 
*/
void listAddContact(List* l,Contact* c){/* dodaje element na koniec */
	ListNode* lastNode=l->tail->prev;
	ListNode* newNode = makeListNode(c);
	newNode->prev=l->tail->prev;
	l->tail->prev=newNode;
	lastNode->next=newNode;
	newNode->next=l->tail;
	l->len++;
}
void listDelContact(List* l,Contact* c){
	assert(l->len!=0);
	ListNode* n=listFindContactNode(l,c);
	assert(n!=NULL);
	n->next->prev=n->prev;
	n->prev->next=n->next;
	delListNode(n);
	l->len--;
}
/*
- wyszukiwanie elementu w ksiazce
*/
ListNode* listFindContactNode(List* l,Contact* c){
	assert(l!=NULL);
	assert(c!=NULL);
	ListNode* n=l->head->next;
	for(int i=0;i<l->len;i++){
		if(areEqualContacts(c,n->data)==0){
			return n;
		}
		n=n->next;
	}
	return NULL;
}
Contact* listFindContactByKey(List *l,ContactKey key,char* val){
	assert(l!=NULL);
	ListNode* n=l->head->next;
	for(int i=0;i<l->len;i++){
		if(cmpContactKeyValue(n->data,key,val)==0){
			return n->data;
		}
		n=n->next;
	}
	return NULL;
}
/*
- sortowanie/przebudowanie ksiazki wg wybranego pola (Nazwisko, Data urodzenia, email, telefon)
*/
void listMerge(ListNode* start,ListNode* mid,int l,int q, int r, ContactKey key){/* warunek posortowane od l do q, od q+1 do r */
    int i=l;
	int j=q+1;
	ListNode* k=start->prev;
	ListNode* a=start;
	ListNode* b=mid;
	ListNode* tmp;
	for(int z=l;z<=r;z++){
		if(i<=q && (j>r || (cmpContacts(a->data,b->data,key)<=0))){
			++i;
			tmp=a;
			a=a->next;
		}
		else{
			++j;
			tmp=b;
			b=b->next;
		}
		k->next=tmp;
		tmp->prev=k;
		k=tmp;
	}
	b->prev=k;
	k->next=b;
}
void listSort(ListNode* start,int l,int r, ContactKey key){
	if(l<r){
		int q=(int)((l+r)/2);
		ListNode* mid=start;
		for(int i=l;i<=q;i++){
			mid=mid->next;
		}
		ListNode* preStart = start->prev;
		listSort(start, l, q, key);/*moze zmienic miedzy l a q*/
		start = preStart->next;
		ListNode* preMid = mid->prev;
		listSort(mid, q + 1, r, key);/* moze zmienic miedzy q+1 a r*/
		mid = preMid->next;
		listMerge(start, mid, l, q, r, key);/* moze zmienic miedzy l a r*/
	}
}
void listSortFull(List* l,ContactKey key){
	listSort(l->head->next,0,l->len-1,key);
}
/* extra */
void listPrint(List* l){
	ListNode* n=l->head->next;
	printf("Printing list with len %d :\n",l->len);
	for(int i=1;i<=l->len;i++){
		printf("Element %d :",i);
		printContact(n->data);
		/*
		printf("his prevnexts:\n");
		if(n->prev->data==NULL){
			printf("its head!\n");
		}
		else{
			printContact(n->prev->data);
		}
		if(n->next->data==NULL){
			printf("its tail!\n");
		}
		else{
			printf(".");
		}
		*/
		n=n->next;
	}
}