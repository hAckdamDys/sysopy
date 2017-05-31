all: compile static dynamic

compile: list.c tree.c contacts.c
	gcc -std=c99 -c contacts.c list.c tree.c
	gcc -std=c99 -fPIC -c -o contacts2.o contacts.c
	gcc -std=c99 -fPIC -c -o list2.o list.c
	gcc -std=c99 -fPIC -c -o tree2.o tree.c
static:
	ar -cr libstatcontacts.a list.o tree.o contacts.o
dynamic:
	gcc -std=c99 -shared -o libdyncontacts.so list2.o tree2.o contacts2.o
