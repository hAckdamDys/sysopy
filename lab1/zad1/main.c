#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "contacts.h"
#include "list.h"
#include "tree.h"

int main ( int argc, char **argv ) {
	Contact *contacts[20];
	for(int i=0;i<20;i++){
		contacts[i]=makeContact();
		char str[5];
		snprintf(str, 5, "%d", (i+1000));
		strcat(str, "-11-02");
		initContact(contacts[i],"adam","dyszy",str,"mail@mail.com","233255255","adres adres");
	}
	List* l1 = makeList();
	delList(l1);
	l1=makeList();
	listAddContact(l1,contacts[6]);
	listAddContact(l1,contacts[3]);
	listAddContact(l1,contacts[2]);
	listAddContact(l1,contacts[11]);
	listAddContact(l1,contacts[12]);
	listAddContact(l1,contacts[1]);
	Contact* contactDif=makeContact();
	initContact(contactDif,"afasd","Fysz","4000-99-99","eil@mail.com","033255255","zdres adres");
	listAddContact(l1,contactDif);
	listAddContact(l1,contacts[7]);
	listAddContact(l1,contacts[0]);
	listAddContact(l1,contacts[13]);
	listAddContact(l1,contacts[5]);
	listAddContact(l1,contacts[4]);
	printf("\npresort:\n\n");
	listPrint(l1);
	printf("\n\naftersort:\n");
	listSortFull(l1,BIRTHDATE);
	listPrint(l1);
	printContact(listFindContactByKey(l1,SURNAME,"Fysz"));
	
	
	printf("\n\nTREES:\n");
	Tree* t1=makeTree(BIRTHDATE);
	treeAddContact(t1,contacts[7]);
	treeAddContact(t1,contacts[8]);
	treeAddContact(t1,contacts[1]);
	treeAddContact(t1,contacts[13]);
	treeAddContact(t1,contacts[18]);
	treeAddContact(t1,contacts[0]);
	treeAddContact(t1,contacts[4]);
	treeDelContact(t1,contacts[0]);
	treeTraversePrint(t1);
	treeDelContact(t1,contacts[7]);
	treeTraversePrint(t1);
	
	Tree* t2=makeTree(BIRTHDATE);
	treeAddContact(t2,contacts[7]);
	treeTraversePrint(t2);
	treeDelContact(t2,contacts[7]);
	treeTraversePrint(t2);
	
	treeAddContact(t1,contacts[4]);
	treeTraversePrint(t1);
	return 0;
}