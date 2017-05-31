CC=gcc
binaries = barber clients
all: $(binaries)

barber: barber.c
	gcc -o $@ barber.c
	
clients: clients.c
	gcc -o $@ clients.c

.PHONY: clean

clean:
	rm -f *.o $(binaries)