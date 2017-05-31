#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>
#include "list.h"
#include "contacts.h"
int main(void) {
 void *handle;
 // Zadeklarowanie zmiennej wskaźnikowej
 List* (*makeList)(void);
 char *error;
 // Otwarcie biblioteki
 handle = dlopen("./libdyncontacts.so", RTLD_LAZY);
 if (!handle) {
 fprintf(stderr, "%s\n", dlerror());
 return 1;
 }
 // Uzyskanie uchwytu do funkcji cos w bibliotece
 makeList = (List* (*)(void)) dlsym(handle, "makeList");


 // Obsługa ewentualnego błędu
 error = dlerror();
 if (error != NULL) {
 fprintf(stderr, "%s\n", error);
 return 1;
 }
 // Użycie funkcji - zauważ operator wyłuskania *
 List* l = (*makeList)();
 // Zamknięcie biblioteki
 dlclose(handle);
 return 0;
}