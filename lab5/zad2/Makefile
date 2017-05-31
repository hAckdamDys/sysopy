CC=gcc
binaries = slave master
all: $(binaries)

master: master.c
	gcc -o $@ $^

slave: slave.c
	gcc -o $@ $^ -lm

.PHONY: clean

clean:
	rm -f *.o $(binaries)