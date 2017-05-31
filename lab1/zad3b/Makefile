all: static dynamic shared
static: 
	gcc -std=c99 -o mains main.c -L. -lstatcontacts
runstatic: 
	./mainst
shared:
	gcc -std=c99 main.c -L. -Wl,-rpath=. -ldyncontacts -o maind
runshared:
	./mainsh
dynamic:
	gcc -std=c99 main3.c -ldl -o maindy
rundynamic:
	./maindy
static_o0: 
	gcc -std=c99 -o0 -o main0 main.c -L. -lstatcontacts
run0:
	./main0
static_o1: 
	gcc -std=c99 -o1 -o main1 main.c -L. -lstatcontacts
run1:
	./main1
static_o2: 
	gcc -std=c99 -o2 -o main3 main.c -L. -lstatcontacts
run2:
	./main2
static_o3: 
	gcc -std=c99 -o3 -o main3 main.c -L. -lstatcontacts
run3:
	./main3
static_os: 
	gcc -std=c99 -os -o mainss main.c -L. -lstatcontacts
runs:
	./mainss