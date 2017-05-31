#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc,char *argv[], char *envp[]){
	if(argc<2){
		printf("Brak podanej zmiennej srodowiskowej do odczytu\n");
		return 1;
	}
	if(getenv(argv[1])==NULL){
		printf("Brak zmiennej srodowiskowej o nazwie %s\n",argv[1]);
		return 0;
	}
	printf("Wartosc zmiennej %s to:%s\n",argv[1],getenv(argv[1]));
	return 0;
}