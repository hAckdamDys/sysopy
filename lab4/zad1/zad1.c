#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
//typedef void (*sighandler_t)(int)
char flagStp=0;
void mySigStpHandler(int sigNum){
	if(flagStp==0){
		flagStp=1;
	}
	else{
		flagStp=0;
	}
}
void mySigIntHandler(int sigNum){
	printf("Odebrano sygnał SIGINT\n");
	exit(0);
}
int main(int argc,char *argv[]){
	signal(SIGTSTP, mySigStpHandler);
	struct sigaction oldIntAct;
	sigaction(SIGINT, NULL,&oldIntAct);
	struct sigaction newIntAct=oldIntAct;
	newIntAct.sa_handler=mySigIntHandler;
	sigaction(SIGINT, &newIntAct,&oldIntAct);
	char a='A';
	while(1){
		if(a<'A' || a>'Z'){
			break;	
		}
		printf("%c",a);
		fflush(stdout);
		sleep(1);
		if(flagStp==0){
			++a;
		}
		else{
			--a;
		}
	}
	printf("\n");
	return 0;
}