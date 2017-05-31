#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <signal.h>
#include <time.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <mqueue.h>

#include "komunikaty.h"


mqd_t klienci[maxClients];
mqd_t mq;
int curKli;



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
		for(int i=0;i<=curKli;i++){
			mq_close(klienci[i]);
		}
		mq_unlink(qName);
		mq=-1;
		exit(0);
	}
}


int main(int argc,char *argv[], char *envp[]){
	atexit(exitBehavior);
	signal(SIGINT, exitBehavior);
	
	struct Zlecenie buf;
	int rozmiar = sizeof(struct Zlecenie);
    struct mq_attr attr;

    /* initialize the queue attributes */
    attr.mq_flags = 0;
    attr.mq_maxmsg = 10;
    attr.mq_msgsize = rozmiar;
    attr.mq_curmsgs = 0;
	
	mq=mq_open(qName, O_CREAT | O_RDWR, 0644, &attr);
	if(mq==-1){
		printf("nie udalo sie stworzyc kolejki\n");
		exit(1);
	}
	//strcpy(buf.wartosc,"RAngom");
	//if(mq_send(mq,(char *)&buf,rozmiar, 0)==-1){
	//	printf("nie udalo sie wyslac wiad\n");
	//}
	//mq_receive(mq, (char*)&buf2, rozmiar, NULL);
	//printf("%s\n",buf2.wartosc);
	//return 0;
	mqd_t tmpQ;
	curKli=-1;//ktory klient
	while(1){
		//msgrcv(id,&buf,rozmiar,0,0);
		mq_receive(mq, (char*)&buf, rozmiar, NULL);
		//printf("%s\n",buf.wartosc);
		if(buf.typ==NOWYKLIENT){
			++curKli;//przechodzimy do kolejnego
			if(curKli>=maxClients){
				printf("za duzo klientow\n");
				--curKli;
				exit(1);
			}
			tmpQ=klienci[curKli]=mq_open(buf.wartosc, O_RDWR, 0644, &attr);
			buf.zid=curKli;
			//wysylamy id przydzielamy do klienta:
			mq_send(tmpQ,(char *)&buf,rozmiar, 0);
			//printf("I got pid %d, an qid %d, servid %d\n",buf.pid,buf.qid,id);
			//msgsnd(tmpQid,&buf,rozmiar,0);
		}
		else if(buf.typ==ECHO){
			tmpQ=klienci[buf.zid];
			mq_send(tmpQ,(char *)&buf,rozmiar, 0);
		}
		else if(buf.typ==WERSALIKI){
			tmpQ=klienci[buf.zid];
			strupp(buf.wartosc);
			mq_send(tmpQ,(char *)&buf,rozmiar, 0);
		}
		else if(buf.typ==KONIEC){
			break;
		}
		else if(buf.typ==CZAS){
			tmpQ=klienci[buf.zid];
			time_t rawtime;
			struct tm *info;
			time( &rawtime );
			info = localtime( &rawtime );
			strftime(buf.wartosc,maxBufLen,"%x - %I:%M:%S%p", info);
			mq_send(tmpQ,(char *)&buf,rozmiar, 0);
		}
		sleep(1);
	}
	struct timespec timeout={time(NULL)+10,0};
	while(1){
		timeout.tv_sec=time(NULL);
		if( mq_timedreceive(mq, (char*)&buf, rozmiar, NULL,&timeout)<0) break;
		//printf("%s\n",buf.wartosc);
		if(buf.typ==NOWYKLIENT){
			++curKli;//przechodzimy do kolejnego
			if(curKli>=maxClients){
				printf("za duzo klientow\n");
				--curKli;
				exit(1);
			}
			tmpQ=klienci[curKli]=mq_open(buf.wartosc, O_RDWR, 0644, &attr);
			buf.zid=curKli;
			//wysylamy id przydzielamy do klienta:
			mq_send(tmpQ,(char *)&buf,rozmiar, 0);
			//printf("I got pid %d, an qid %d, servid %d\n",buf.pid,buf.qid,id);
			//msgsnd(tmpQid,&buf,rozmiar,0);
		}
		else if(buf.typ==ECHO){
			tmpQ=klienci[buf.zid];
			mq_send(tmpQ,(char *)&buf,rozmiar, 0);
		}
		else if(buf.typ==WERSALIKI){
			tmpQ=klienci[buf.zid];
			strupp(buf.wartosc);
			mq_send(tmpQ,(char *)&buf,rozmiar, 0);
		}
		else if(buf.typ==KONIEC){
			break;
		}
		else if(buf.typ==CZAS){
			tmpQ=klienci[buf.zid];
			time_t rawtime;
			struct tm *info;
			time( &rawtime );
			info = localtime( &rawtime );
			strftime(buf.wartosc,maxBufLen,"%x - %I:%M:%S%p", info);
			mq_send(tmpQ,(char *)&buf,rozmiar, 0);
		}
	}
	return 0;
}
