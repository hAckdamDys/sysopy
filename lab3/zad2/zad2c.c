#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <sys/resource.h>

int main(int argc,char *argv[], char *envp[]){
	//program trwa okolo 2952000 mikrosekund czyli dla ograniczenia 1s sie wywala
	int a=2;
	while(1){
		a+=1;
		if(a>1000000000){
			break;
		}
	}
	struct rlimit timeLim;
	struct rlimit virMemLim;
	getrlimit(RLIMIT_AS, &virMemLim);
	getrlimit(RLIMIT_CPU, &timeLim);
	printf("Zad2c: %d petli, z limitem czasu: %ld sekund, limitem pamiec: %f MB\n",a,(long)(timeLim.rlim_max),(double)(virMemLim.rlim_max/(1024*1024)));
	return 0;
}