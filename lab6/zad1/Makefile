CC=gcc
binaries = server klient
all: $(binaries)

server: server.c komunikaty.h
	gcc -o $@ server.c
	
klient: klient.c komunikaty.h
	gcc -o $@ klient.c

.PHONY: clean

clean:
	rm -f *.o $(binaries)