#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
int childPid;
int parentPid;
int isChildEnd=0;
int numUserSigs=0;//liczba ktore dostal potomek
int SIGRT1;
int SIGRT2;
void sigIntHdlr(int sigNum){
	if(getpid()!=parentPid){
		printf("Dziecko dostalo %d syngalow i teraz sigint wiec konczy prace\n",numUserSigs);
		exit(1);
	}
	printf("Parent dostal sygnal sigint i konczy prace\n");
	exit(1);
}
void sigUsr1Hdlr(int sigNum){
	numUserSigs++;
	if(getpid()!=parentPid){//dziecko wysyla spowrotem
		kill(parentPid,SIGUSR1);
	}
}
void sigUsr2Hdlr(int sigNum){
	if(getpid()!=parentPid){
		printf("Dziecko dostalo %d sygnalow sigusr1 i konczy dzialanie bo dostalo sigusr2\n",numUserSigs);
		exit(0);
	}
}
void sigRlt1Hdlr(int sigNum){
	numUserSigs++;
	if(getpid()!=parentPid){//dziecko wysyla spowrotem
		kill(parentPid,sigNum);
	}
}
void sigRlt2Hdlr(int sigNum){
	if(getpid()!=parentPid){
		printf("Dziecko dostalo %d sygnalow sigrlt1 i konczy dzialanie bo dostalo sigrlt2\n",numUserSigs);
		exit(0);
	}
}
int main(int argc,char *argv[]){
	if(argc<3){
		printf("Podaj N ilosc potomkow L ilosc sygnalow i T czas miedzy potomkami\n");
		return 1;
	}
	srand(time(NULL));
	int N=atoi(argv[1]);
	int L=atoi(argv[2]);
	int T=0;
	if(argc>3){
		T=atoi(argv[3]);
	}
	signal(SIGINT, sigIntHdlr);
	signal(SIGUSR1,sigUsr1Hdlr);
	signal(SIGUSR2, sigUsr2Hdlr);
	for(int i=SIGRTMIN;i<SIGRTMIN+16;i++){
		signal(i, sigRlt1Hdlr);
	}
	for(int i=SIGRTMIN+16;i<SIGRTMAX;i++){
		signal(i, sigRlt2Hdlr);
	}
	parentPid=getpid();
	int* childPids=malloc(sizeof(int)*N);
	for(int i=0;i<N;i++){
		childPid=fork();
		if(childPid==0){//potomek
			childPid=getpid();
			//printf("childPid = %d\n",childPid);
			for(int i=0;i<2*N*T;i++){//zywotnosc 2*N*T sekund max
				sleep(1);
			}
			exit(1);
		}
		else{
			childPids[i]=childPid;
		}
	}
	int tmpPidId;
	int tmpPid;
	for(int j=0;j<6;j++){//6 razy shuffle
		for(int i=0;i<N-1;i++){//shuffle childpidów
			tmpPidId=(i+1+(rand()%(N-1)))%N;
			tmpPid=childPids[i];
			childPids[i]=childPids[tmpPidId];
			childPids[tmpPidId]=tmpPid;
		}
	}
	for(int i=0;i<N;i++){
		int Type=1+rand()%4;
		numUserSigs=0;
		SIGRT1=SIGRTMIN+rand()%16;
		SIGRT2=SIGRTMIN+16+rand()%(SIGRTMAX-SIGRTMIN-16);
		childPid=childPids[i];
		//kod z zad3
		printf("\n\nType = %d, SIGRT1 = %d, SIGRT2 = %d\n",Type,SIGRT1,SIGRT2);
		int sigsSent=0;
		//printf("childPid at parent = %d\n",childPid);
		if(Type==1){
			for(int i=0;i<L;i++){
				if(kill(childPid,SIGUSR1)==0){
					sigsSent++;
				}
			}
			if(kill(childPid,SIGUSR2)==0){
				sigsSent++;
			}
			printf("Parent wyslal %d sygnalow\n",sigsSent);
		}
		else if(Type==2){
			const union sigval tmp={42,NULL};
			for(int i=0;i<L;i++){
				if(sigqueue(childPid,SIGUSR1,tmp)==0){
					sigsSent++;
				}
				else{
					switch(errno){
						case EAGAIN:
							printf("he limit of signals which may be queued has been reached.\n");
							break;
						case EINVAL:
							printf("sig was invalid.\n");
							break;
						case EPERM:
							printf("he process does not have permission to send the signal to the receiving process.\n");
							break;
						case ESRCH:
							printf("No process has a PID matching pid.\n");
							break;
						default:
							printf("LOL?\n");
							break;
					}
					
				}
			}
			if(sigqueue(childPid,SIGUSR2,tmp)==0){
				sigsSent++;
			}
			printf("Parent wyslal %d sygnalow\n",sigsSent);
		}
		else if(Type==3){
			for(int i=0;i<L;i++){
				if(kill(childPid,SIGRT1)==0){
					sigsSent++;
				}
			}
			if(kill(childPid,SIGRT2)==0){
				sigsSent++;
			}
			printf("Parent wyslal %d sygnalow\n",sigsSent);
		}
		else{
			const union sigval tmp={42,NULL};
			for(int i=0;i<L;i++){
				if(sigqueue(childPid,SIGRT1,tmp)==0){
					sigsSent++;
				}
				else{
					switch(errno){
						case EAGAIN:
							printf("he limit of signals which may be queued has been reached.\n");
							break;
						case EINVAL:
							printf("sig was invalid.\n");
							break;
						case EPERM:
							printf("he process does not have permission to send the signal to the receiving process.\n");
							break;
						case ESRCH:
							printf("No process has a PID matching pid.\n");
							break;
						default:
							printf("LOL?\n");
							break;
					}
					
				}
			}
			if(sigqueue(childPid,SIGRT2,tmp)==0){
				sigsSent++;
			}
			printf("Parent wyslal %d sygnalow\n",sigsSent);
		}
		while(1){
			if(wait(NULL)!=0){
				sleep(1);
				printf("Parent otrzymal %d sygnalow\n",numUserSigs);
				break;
			}
		}
		sleep(T);
	}
	return 0;
}