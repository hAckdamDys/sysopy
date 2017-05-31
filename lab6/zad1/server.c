#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/shm.h>
#include <unistd.h>
#include <ctype.h>
#include <signal.h>
#include <time.h>

#include "komunikaty.h"

struct Klienci{
	int pid;
	int qid;
};

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
	atexit(exitBehavior);
	signal(SIGINT, exitBehavior); 
	
	const char* home = getenv("HOME");
	struct Klienci klienci[maxClients];
	key_t klucz = ftok(home,kluczGen);
	if(klucz == -1){
		printf("error on ftok\n");
		exit(1);
	}
	struct msqid_ds staty;
	id=msgget(klucz,IPC_CREAT | 0600);//tworzy kolejke IPC z dostepem 0600
	struct Zlecenie buf;
	int rozmiar = sizeof(struct Zlecenie)-sizeof(long);
	int tmpQid;
	int curKli=-1;//ktory klient
	while(1){
		msgrcv(id,&buf,rozmiar,0,0);
		//printf("%s\n",buf.wartosc);
		if(buf.typ==NOWYKLIENT){
			++curKli;//przechodzimy do kolejnego
			if(curKli>=maxClients){
				printf("za duzo klientow\n");
				exit(1);
			}
			klienci[curKli].pid=buf.pid;
			klienci[curKli].qid=tmpQid=buf.qid;
			//printf("I got pid %d, an qid %d, servid %d\n",buf.pid,buf.qid,id);
			buf.qid=curKli;
			msgsnd(tmpQid,&buf,rozmiar,0);
		}
		else if(buf.typ==ECHO){
			tmpQid=buf.qid;
			buf.pid=getpid();
			buf.qid=id;
			msgsnd(tmpQid,&buf,rozmiar,0);
		}
		else if(buf.typ==WERSALIKI){
			tmpQid=buf.qid;
			strupp(buf.wartosc);
			buf.pid=getpid();
			buf.qid=id;
			msgsnd(tmpQid,&buf,rozmiar,0);
		}
		else if(buf.typ==KONIEC){
			break;
		}
		else if(buf.typ==CZAS){
			tmpQid=buf.qid;
			buf.pid=getpid();
			buf.qid=id;
			time_t rawtime;
			struct tm *info;
			time( &rawtime );
			info = localtime( &rawtime );
			strftime(buf.wartosc,maxBufLen,"%x - %I:%M:%S%p", info);
			msgsnd(tmpQid,&buf,rozmiar,0);
		}
		sleep(1);
	}
	while(1){
		if(msgrcv(id,&buf,rozmiar,0,IPC_NOWAIT)<0) break;
		int tmpQid = buf.qid;
		if(buf.typ==NOWYKLIENT){
			++curKli;//przechodzimy do kolejnego
			if(curKli>=maxClients){
				printf("za duzo klientow\n");
				exit(1);
			}
			klienci[curKli].pid=buf.pid;
			klienci[curKli].qid=tmpQid=buf.qid;
			//printf("I got pid %d, an qid %d, servid %d\n",buf.pid,buf.qid,id);
			buf.qid=curKli;
			msgsnd(tmpQid,&buf,rozmiar,0);
		}
		buf.pid=getpid();
		buf.qid=id;
		if(buf.typ==CZAS){
			time_t rawtime;
			struct tm *info;
			time( &rawtime );
			info = localtime( &rawtime );
			strftime(buf.wartosc,maxBufLen,"%x - %I:%M:%S%p", info);
			msgsnd(tmpQid,&buf,rozmiar,0);
		}
		else if(buf.typ!=KONIEC){
			if(buf.typ==WERSALIKI){
				strupp(buf.wartosc);
			}
			msgsnd(tmpQid,&buf,rozmiar,0);
		}
	}
	return 0;
}
