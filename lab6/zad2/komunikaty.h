#ifndef KOMUNIKATY_H
#define KOMUNIKATY_H

#include <stdlib.h>

#define msgSize 4096

#define maxBufLen 32

#define kluczGen 254

#define maxClients 32

#define qName "/server_queue"

struct Zlecenie{
	long typ;
	char wartosc[maxBufLen];
	int pid;
	int zid;//id przydzielone
};

enum RodzajZlecenia{
	NOWYKLIENT=1,ECHO,WERSALIKI,CZAS,KONIEC
};

#endif