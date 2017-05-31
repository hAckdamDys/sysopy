#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/shm.h>
#include <unistd.h>
#include <ctype.h>
#include <time.h>
#include <signal.h>

#include "komunikaty.h"

int id=-1;

void strupp(char* beg)
{
    while (*beg){
		*beg = toupper(*beg);
		++beg;
	}
}

void exitBehavior(){
	if(id!=-1){
		printf("proces %d usuwa kolejke %d\n",getpid(),id);
		msgctl(id, IPC_RMID, NULL);//usuwa kolejke
		id=-1;
		exit(0);
	}
}

int main(int argc,char *argv[], char *envp[]){
	//usuniecie kolejki w razie zamkniecia:
	atexit(exitBehavior);
	signal(SIGINT, exitBehavior); 
	
	const char* home = getenv("HOME");
	//swoja kolejka
	key_t klucz = ftok(home,getpid()%kluczGen);
	if(klucz == -1){
		printf("error on self ftok\n");
		exit(1);
	}
	id=msgget(klucz, IPC_CREAT |IPC_PRIVATE | 0600);//tworzy kolejke IPC z dostepem 0600
	//kolejka servera
	klucz = ftok(home,kluczGen);
	if(klucz == -1){
		printf("error on servers ftok\n");
		exit(1);
	}
	int servId=msgget(klucz, 0600);//tworzy kolejke IPC z dostepem 0600
	if(servId ==-1){
		printf("Server jest wylaczony\n");
		exit(1);
	}
	struct Zlecenie buf;
	int rozmiar = sizeof(struct Zlecenie)-sizeof(long);
	//wysylamy do servera swoj klucz
	buf.typ=NOWYKLIENT;
	buf.qid=id;
	buf.pid=getpid();
	strcpy(buf.wartosc,"KLIENT");
	//printf("my pid %d, and qid %d, servid %d\n",getpid(),id,servId);
	msgsnd(servId,&buf,rozmiar,0);
	msgrcv(id,&buf,rozmiar,0,0);
	int myGivenId = buf.qid;
	printf("I got %d id\n",myGivenId);
	//return 0;
	
	srand(((unsigned)getpid() << 16) | (0xFFFFu & (unsigned)time(NULL)));
	while(1){
		buf.typ=ECHO+rand()%3;
		//buf.wartosc[3]=buf.typ+48;
		if(buf.typ==KONIEC){
			msgsnd(servId,&buf,rozmiar,0);
			break;
		}
		else if(buf.typ==ECHO || buf.typ==WERSALIKI){
			strcpy(buf.wartosc,"maleDUZEo");
			buf.wartosc[8]=97+rand()%26;//ostatnia literka losowo
		}
		buf.qid=id;
		buf.pid=getpid();
		msgsnd(servId,&buf,rozmiar,0);
		char timeout=1;
		for(int i=0;i<10;i++){
			if(msgrcv(id,&buf,rozmiar,0,IPC_NOWAIT)<0){
				sleep(1);
			}
			else{
				timeout=0;
				break;
			}
		}
		if(timeout){
			printf("timeout\n");
			exit(1);
		}
		printf("%s\n",buf.wartosc);
		//sleep(0.1);
	}
	return 0;
}
