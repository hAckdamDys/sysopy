#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <signal.h>
#include <time.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <unistd.h>
#include <sys/stat.h>
#include <mqueue.h>

#include "komunikaty.h"

mqd_t mq=-1;
char myQname[maxBufLen];
void strupp(char* beg)
{
    while (*beg){
		*beg = toupper(*beg);
		++beg;
	}
}

void exitBehavior(){
	if(mq!=-1){
		printf("proces %d usuwa kolejke %d\n",getpid(),mq);
		//msgctl(id, IPC_RMID, NULL);//usuwa kolejke
		mq_close(mq);
		mq_unlink(myQname);
		mq=-1;
		exit(0);
	}
}

int main(int argc,char *argv[], char *envp[]){
	//usuniecie kolejki w razie zamkniecia:
	atexit(exitBehavior);
	signal(SIGINT, exitBehavior); 
	
	const char* home = getenv("HOME");
	struct Zlecenie buf;
	int rozmiar = sizeof(struct Zlecenie);
    struct mq_attr attr;

    /* initialize the queue attributes */
    attr.mq_flags = 0;
    attr.mq_maxmsg = 10;
    attr.mq_msgsize = rozmiar;
    attr.mq_curmsgs = 0;
	//kolejka servera
	mqd_t servMq=mq_open(qName, O_RDWR, 0644, &attr);
	if(servMq==-1){
		printf("nie udalo sie stworzyc kolejki serva\n");
		exit(1);
	}
    
	//swoja kolejka
	key_t klucz = ftok(home,getpid()%kluczGen);
	if(klucz == -1){
		printf("error on self ftok\n");
		exit(1);
	}
    myQname[0]='/';
    snprintf(myQname+sizeof(char), maxBufLen-1, "%d", klucz);
	mq=mq_open(myQname, O_CREAT | O_RDWR, 0644, &attr);
	if(servMq==-1){
		printf("nie udalo sie stworzyc kolejki klienta\n");
		exit(1);
	}
    
    
	//wysylamy do servera swoj klucz
	buf.typ=NOWYKLIENT;
	strcpy(buf.wartosc,myQname);
	buf.pid=getpid();
	//strcpy(buf.wartosc,"KLIENT");
	//printf("my pid %d, and qid %d, servid %d\n",getpid(),id,servId);
    mq_send(servMq,(char *)&buf,rozmiar, 0);
    struct timespec timeout={time(NULL)+10,0};
    if( mq_timedreceive(mq, (char*)&buf, rozmiar, NULL, &timeout)<0){
        printf("timeout\n");
        exit(1);
    }
	int myGivenId = buf.zid;
	printf("I got %d id\n",myGivenId);
	//return 0;
	
	srand(((unsigned)getpid() << 16) | (0xFFFFu & (unsigned)time(NULL)));
	while(1){
		buf.typ=ECHO+rand()%4;
		//buf.wartosc[3]=buf.typ+48;
		if(buf.typ==KONIEC){
            buf.zid=myGivenId;
			mq_send(servMq,(char *)&buf,rozmiar, 0);
			break;
		}
		else if(buf.typ==ECHO || buf.typ==WERSALIKI){
			strcpy(buf.wartosc,"maleDUZEo");
			buf.wartosc[8]=97+rand()%26;//ostatnia literka losowo
		}
		buf.zid=myGivenId;
		buf.pid=getpid();
		mq_send(servMq,(char *)&buf,rozmiar, 0);
        timeout.tv_sec=time(NULL)+10;
        if(mq_timedreceive(mq, (char*)&buf, rozmiar, NULL, &timeout)<0){
            printf("timeout\n");
            exit(1);
        }
		printf("%s\n",buf.wartosc);
		//sleep(0.1);
	}
	return 0;
}
