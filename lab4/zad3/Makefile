CC=gcc
binaries = zad3 zad3b
all: $(binaries)

zad3: zad3.c
	gcc -o $@ $^

zad3b: zad3b.c
	gcc -o $@ $^

.PHONY: clean

clean:
	rm -f *.o $(binaries)