CC=gcc
libs = -lpthread
binaries = zad1a zad1b zad1c
all: $(binaries)
	
zad1a: zad1a.c
	gcc -o $@ zad1a.c $(libs)
	
zad1b: zad1b.c
	gcc -o $@ zad1b.c $(libs)
	
zad1c: zad1c.c
	gcc -o $@ zad1c.c $(libs)

.PHONY: clean

clean:
	rm -f *.o $(binaries)