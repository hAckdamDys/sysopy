#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdio.h>

#include "contacts.h"

Contact* makeContact(){
    Contact *c = malloc(sizeof(Contact));

    c->name = malloc(32);
    c->surName = malloc(32);
    c->birthDate = malloc(10);
    c->email = malloc(50);
    c->phone = malloc(15);
    c->address = malloc(100);

    return c;
}

void initContact(Contact* c,char* name,char* surName,char* birthDate,char* email,char* phone,char* address){
	assert(c!=NULL);
	strcpy(c->name,name);
	strcpy(c->surName,surName);
	strcpy(c->birthDate,birthDate);
	strcpy(c->email,email);
	strcpy(c->phone,phone);
	strcpy(c->address,address);
}

int cmpContactKeyValue(Contact* a,ContactKey key,char* val){
	assert(a!=NULL);
	switch (key) {
		case SURNAME:
			/*printf("dana:%s, szukana:%s\n",a->surName,val);*/
			return strcmp(a->surName,val);
		case BIRTHDATE:
			return strcmp(a->birthDate,val);
		case EMAIL:
			return strcmp(a->email,val);
		case PHONE:
			return strcmp(a->phone,val);
    }
	return 0;
}

int cmpContacts(Contact* a,Contact* b,ContactKey key){
	assert(a!=NULL);
	assert(b!=NULL);
	switch (key) {
		case SURNAME:
			return strcmp(a->surName,b->surName);
		case BIRTHDATE:
			return strcmp(a->birthDate,b->birthDate);
		case EMAIL:
			return strcmp(a->email,b->email);
		case PHONE:
			return strcmp(a->phone,b->phone);
    }
	return 0;
}

int areEqualContacts(Contact* a,Contact* b){/* jesli w kontakcie a są nulle to nie porownujemy tych w b */
	if(a==b){
		return 0;
	}
	assert(a!=NULL);
	assert(b!=NULL);
	if(a->name!=NULL){
		if(strcmp(a->name,b->name)!=0){
			return 1;
		}
	}
	if(a->surName!=NULL){
		if(strcmp(a->surName,b->surName)!=0){
			return 1;
		}
	}
	if(a->birthDate!=NULL){
		if(strcmp(a->birthDate,b->birthDate)!=0){
			return 1;
		}
	}
	if(a->email!=NULL){
		if(strcmp(a->email,b->email)!=0){
			return 1;
		}
	}
	if(a->phone!=NULL){
		if(strcmp(a->phone,b->phone)!=0){
			return 1;
		}
	}
	if(a->address!=NULL){
		if(strcmp(a->address,b->address)!=0){
			return 1;
		}
	}
	return 0;
}

void delContact(Contact* c){
	assert(c!=NULL);
	free(c->name);
	free(c->surName);
	free(c->birthDate);
	free(c->email);
	free(c->phone);
	free(c->address);
	
	free(c);
}

void printContact(Contact* c){
	printf("Contact: %s",c->name);
	printf(", %s",c->surName);
	printf(", %s",c->birthDate);
	printf(", %s",c->email);
	printf(", %s",c->phone);
	printf(", %s\n",c->address);
}