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
//typedef void (*sighandler_t)(int)
//void (*sa_sigaction)(int, siginfo_t *, void *);

int gotSignals=0;//ilosc sygnalow sigUsr ktore dostal parent
int kSigs;//ilosc sygnalow ktore parent musi dostac od dzieci
char isFree = 0;//czy dostalismy pozwolenie czyli czy parent dostal kSigs signali sigUsr od potomkow
int* childrenPids;//przechowujemy Pid potomkow
int globKid=-1;//ktory bierząco potomek byl sprawdzany zaczyna sie od -1 bo w petli jest odrazu ++
int mainPid=-2;//pid parenta, init na -2 zeby sygnalow nie wysylaly gdzies w glupie miejsce jak by byl bug
int childStop=0;//0 init, 1 gdy dziecko dostanie pozwolenie od rodzica
int howManyRlts=0;//ilosc sygnalow rlt jakie dostal parent
int childNum=-2;//numer dziecka
static int min(int a,int b){
	return (a<b)?a:b;
}

void sigUsrHdlr(int sigNum,siginfo_t* info,void* context){
	if(getpid()!=mainPid){//jak dziecko dostanie sigUsr czyli pozwolenie to ustawiamy mu childStop na 1 i wtedy konczy petle
		childStop=1;
		return;
	}
	globKid++;//zwiekszamy indeks dziecka ktore sprawdzamy
	childrenPids[globKid]=info->si_pid;
	if(isFree==1){//jak jestesmy wolni to automatycznie odrazu wysylamy
		kill(childrenPids[globKid],SIGUSR1);
	}
	else{//jak jeszcze nie bo isFree=0
		gotSignals+=1;
		if(gotSignals>=kSigs){//jak zwalniamy to musimy wyslac wszystkim poprzednim pozwolenie
			isFree=1;
			for(int i=0;i<=globKid;i++){
				kill(childrenPids[i],SIGUSR1);
			}
		}
	}
}
void sigRltHdlr(int sigNum,siginfo_t* info,void* context){
	howManyRlts++;
	printf("Dostalem %d rlt signal z numerem \"%d\" od %d\n",howManyRlts,sigNum,info->si_pid);
}

void sigIntHdlr(int sigNum){
	if(getpid()!=mainPid){
		printf("Dziecko %d o procesie %d dostalo sygnal sigint i konczy prace\n",childNum,getpid());
		exit(0);
	}
	for(int i=0;i<=globKid;i++){
		if(childrenPids[i]!=0){
			kill(childrenPids[i],SIGINT);
		}
	}
	printf("Parent %d dostal sygnal sigint i konczy prace\n",mainPid);
	exit(0);
}

int main(int argc,char *argv[]){
	if(argc!=3){
		printf("Podaj N potomnkow i K prosb\n");
		return 1;
	}
	int N=atoi(argv[1]);
	kSigs=atoi(argv[2]);
	if(kSigs>N){
		printf("Nie mozliwe zeby bylo wiecej prosb niz potomkow!\n");
		exit(1);
	}
	childrenPids=calloc(sizeof(int),N);
	mainPid=getpid();
	struct sigaction oldAct;
	sigaction(SIGUSR1, NULL,&oldAct);
	struct sigaction newAct=oldAct;
	newAct.sa_flags=SA_SIGINFO;
	newAct.sa_sigaction=sigUsrHdlr;
	sigaction(SIGUSR1, &newAct,NULL);
	//rlt sigs:
	for (int i= SIGRTMIN;i<=SIGRTMAX;i++){
		sigaction(i, NULL,&oldAct);
		struct sigaction newRltAct=oldAct;
		newRltAct.sa_flags=SA_SIGINFO;
		newRltAct.sa_sigaction=sigRltHdlr;
		sigaction(i, &newRltAct,NULL);
	}
	//sigint
	signal(SIGINT, sigIntHdlr);
	for(int curKid=0;curKid<N;curKid++){
		if(fork()==0){//potomny
			childNum=curKid;
			srand(time(NULL) ^ (getpid()<<16));
			int ileSpal = rand()%11;
			sleep(ileSpal);//(uśnie na 0-10 sekund)
			//int kill(pid_t pid, int sig)
			//wysyła sygnał SIGUSR1 prośby o przejście
			time_t timePre=time(NULL);
			kill(getppid(),SIGUSR1);
			for(int i=0;i<20;i++){//potomki czekaja maksymalnie 20sekund
				sleep(1);
				if(childStop==1){
					break;
				}
			}
			if(childStop!=1){
				printf("potomek %d : proces %d : czekal %d sekund i nie dostal pozwolenia\n",curKid,getpid(),(int)(time(NULL)-timePre));
				exit(1);
			}
			// losuje jeden z 32 sygnałów czasu rzeczywistego  (SIGRTMIN,SIGRTMAX),
			kill(getppid(),(SIGRTMIN+(rand()%(SIGRTMAX-SIGRTMIN))));
			printf("potomek %d : proces %d : czekal %d sekund, spal %d\n",curKid,getpid(),(int)(time(NULL)-timePre),ileSpal);
			exit(0);
		}
		else{//parent
			
		}
	}
	//czekamy na wszystkie sygnaly
	int ilePotomkow=0;
	int maxWait=30;//maksymalnie czeka 30sekund
	while(maxWait>0 && ilePotomkow<N){
		if(wait(NULL)!=-1){//wchodzimy tu gdy jakis potomek skonczyl
			ilePotomkow++;
		}
		else{
			maxWait--;
			sleep(1);
		}
	}
	if(ilePotomkow!=N){
		printf("ktorys z potomkow sie nie zakonczyl! wysylam sobie siginta zeby zakonczyc wszystkie\n");
		raise(SIGINT);
		exit(1);
	}
	return 0;
}