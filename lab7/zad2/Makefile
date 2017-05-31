CFLAGS=-pthread -lrt

all: clients barber

clients: clients.c fifo.c util.c communication.h
	gcc -o clients clients.c $(CFLAGS) 
barber: barber.c fifo.c util.c communication.h
	gcc -o barber barber.c $(CFLAGS)

clean:
	rm -f clients barber

.PHONY: all clients barber clean
