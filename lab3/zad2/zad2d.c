#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <sys/resource.h>

int main(int argc,char *argv[], char *envp[]){
	//program alokuje
	if(argc!=2){
		printf("Podaj ilosc MB\n");
		return 1;
	}
	long long int theSize;
	sscanf(argv[1], "%lld", &theSize);
	char* a=calloc(1,1024*1024*theSize);
	if(a!=NULL){
		printf("Zad2d: alokacja %lld MB sie powiodla",theSize);
	}
	else{
		printf("Zad2d: alokacja %lld MB sie nie powiodla",theSize);
	}
	
	struct rlimit timeLim;
	struct rlimit virMemLim;
	getrlimit(RLIMIT_AS, &virMemLim);
	getrlimit(RLIMIT_CPU, &timeLim);
	printf(", z limitem czasu: %ld sekund, limitem pamiec: %f MB\n",(long)(timeLim.rlim_max),(double)(virMemLim.rlim_max/(1024*1024)));
	return 0;
}