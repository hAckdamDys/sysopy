#ifndef CONTACTS_H
#define CONTACTS_H

#include <stdlib.h>

typedef struct Contact{
	char* name;
	char* surName;
	char* birthDate;/* w postaci rok-miesiac-dzien */
	char* email;
	char* phone;
	char* address;
}Contact;

typedef enum ContactKey{
	SURNAME,BIRTHDATE,EMAIL,PHONE
}ContactKey;

Contact* makeContact();
void delContact(Contact* c);

void initContact(Contact* c,char* name,char* surName,char* birthDate,char* email,char* phone,char* address);
int areEqualContacts(Contact* a,Contact* b);
int cmpContactKeyValue(Contact* a,ContactKey key,char* val);
int cmpContacts(Contact* a,Contact* b,ContactKey key);

void printContact(Contact* c);
#endif