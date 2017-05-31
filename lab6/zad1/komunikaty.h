#ifndef KOMUNIKATY_H
#define KOMUNIKATY_H

#include <stdlib.h>

#define maxBufLen 32

#define kluczGen 254

#define maxClients 32

struct Zlecenie{
	long typ;
	char wartosc[maxBufLen];
	int qid;
	int pid;
};

enum RodzajZlecenia{
	NOWYKLIENT=1,ECHO,WERSALIKI,CZAS,KONIEC
};

#endif