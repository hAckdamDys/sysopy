CC=gcc
binaries = server klient
LDLIBS=-lrt
all: $(binaries)

server: server.c komunikaty.h
	gcc -o $@ server.c $(LDLIBS)
	
klient: klient.c komunikaty.h
	gcc -o $@ klient.c $(LDLIBS)

.PHONY: clean

clean:
	rm -f *.o $(binaries)