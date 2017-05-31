#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include <sys/times.h>
#include <time.h>
#include <sys/resource.h>

#include "contacts.h"
#include "list.h"
#include "tree.h"

#include <dlfcn.h>


static char *rand_string(char *str, size_t size)
{
    const char charset[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJK...";
    if (size) {
        --size;
        for (size_t n = 0; n < size; n++) {
            int key = rand() % (int) (sizeof charset - 1);
            str[n] = charset[key];
        }
        str[size] = '\0';
    }
    return str;
}

void showTime(struct timeval user0,	struct timeval system0,struct timespec * real0,struct rusage * usage){
	clock_gettime(CLOCK_REALTIME, real0);
	getrusage(RUSAGE_SELF,usage);
	user0 = usage->ru_utime;
	system0 = usage->ru_stime;
	printf("\nStarting program\nreal:%lld\nuser:%ld\nsystem:%ld\n",
		(long long)real0->tv_sec*1000000+real0->tv_nsec/1000,
		user0.tv_sec*1000000+user0.tv_usec,
		system0.tv_sec*1000000+system0.tv_usec
	);
}

int main ( int argc, char **argv ) {
	void *handle;
	// Zadeklarowanie zmiennej wskaźnikowej
	Contact* (*makeContact)(void);
	void (*initContact)(Contact*,char*,char*,char*,char*,char*,char*);
	
	
	
	List* (*makeList)(void);
	void (*listAddContact)(List*,Contact*);
	void (*listDelContact)(List*,Contact*);
	ListNode* (*listFindContactNode)(List*,Contact*);
	void (*listSortFull)(List*,ContactKey);
	
	
	
	
	
	
	
	
	char *error;
	// Otwarcie biblioteki
	handle = dlopen("./libdyncontacts.so", RTLD_LAZY);
	if (!handle) {
	fprintf(stderr, "%s\n", dlerror());
		return 1;
	}
	// Uzyskanie uchwytu do funkcji cos w bibliotece
	makeContact=(Contact* (*)(void))dlsym(handle,"makeContact");

	initContact=(void (*)(Contact*,char*,char*,char*,char*,char*,char*))dlsym(handle,"initContact");

	
	
	makeList = (List* (*)(void)) dlsym(handle, "makeList");
	listAddContact= (void (*)(List*,Contact*))dlsym(handle, "listAddContact");
	listDelContact= (void (*)(List*,Contact*))dlsym(handle, "listDelContact");
	listFindContactNode= (ListNode* (*)(List*,Contact*))dlsym(handle, "listFindContactNode");
	listSortFull= (void (*)(List*,ContactKey))dlsym(handle, "listSortFull");
	
	
	
	
	
	struct timespec * real0 = malloc(sizeof(struct timespec));
	struct timeval user0;
	struct timeval system0;
	struct rusage * usage = malloc(sizeof(struct timespec));
	Contact *contacts[1000];
	for(int i=0;i<1000;i++){
		contacts[i]=makeContact();
		char str[20];
		initContact(contacts[i],rand_string(str,20),rand_string(str,20),rand_string(str,20),rand_string(str,20),rand_string(str,20),rand_string(str,20));
		//printContact(contacts[i]);
	}
	/* czas dla dodawania elementow */
	List* l=makeList();
	showTime(user0,system0,real0,usage);
	for(int i=1;i<1000;i++){
		listAddContact(l, contacts[i]);
		showTime(user0,system0,real0,usage);
	}
	printf("\n^list dodawania, teraz lista odejmowanie pesy a potem opty:\n");
	showTime(user0,system0,real0,usage);
	listDelContact(l,contacts[500]);
	showTime(user0,system0,real0,usage);
	listDelContact(l,contacts[999]);
	showTime(user0,system0,real0,usage);
	printf("\nteraz lista wyszukiwanie pesy a potem opty:\n");
	showTime(user0,system0,real0,usage);
	listFindContactNode(l,contacts[499]);
	showTime(user0,system0,real0,usage);
	listFindContactNode(l,contacts[998]);
	showTime(user0,system0,real0,usage);
	printf("\nlista sort:\n");
	showTime(user0,system0,real0,usage);
	listSortFull(l,SURNAME);
	showTime(user0,system0,real0,usage);
	
	/*teraz drzewo 
	Tree* t = makeTree(SURNAME);
	for(int i=1;i<1000;i++){
		treeAddContact(t, contacts[i]);
		showTime(user0,system0,real0,usage);
	}
	printf("\n^tree dodawania, teraz lista odejmowanie pesy a potem opty:\n");
	showTime(user0,system0,real0,usage);
	treeDelContact(t,contacts[500]);
	showTime(user0,system0,real0,usage);
	treeDelContact(t,contacts[999]);
	showTime(user0,system0,real0,usage);
	printf("\nteraz tree wyszukiwanie pesy a potem opty:\n");
	showTime(user0,system0,real0,usage);
	treeFindContactNode(t,contacts[499]);
	showTime(user0,system0,real0,usage);
	treeFindContactNode(t,contacts[998]);
	showTime(user0,system0,real0,usage);
	*/
	//printf("\ntree rebuild:\n");
	//showTime(user0,system0,real0,usage);
	//treeTraverseRemake(t,BIRTHDATE);
	//showTime(user0,system0,real0,usage);
	
	
	error = dlerror();
	if (error != NULL) {
		fprintf(stderr, "%s\n", error);
		return 1;
	}
	
	dlclose(handle);
	return 0;
}