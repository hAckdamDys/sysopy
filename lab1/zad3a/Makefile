all: static dynamic shared
static: 
	gcc -std=gnu99 -o mains main.c -L. -lstatcontacts
	./mainst
shared:
	gcc -std=gnu99 main.c -L. -Wl,-rpath=. -ldyncontacts -o maind
	./mainsh
dynamic:
	gcc -std=gnu99 main3.c -ldl -o maindy
	./maindy