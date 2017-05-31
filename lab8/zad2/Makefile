CC=gcc
libs = -lpthread
binaries = zad2a zad2b
all: $(binaries)
	
zad2a: zad2a.c
	gcc -o $@ zad2a.c $(libs)
	
zad2b: zad2b.c
	gcc -o $@ zad2b.c $(libs)
	
.PHONY: clean

clean:
	rm -f *.o $(binaries)