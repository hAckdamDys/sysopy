#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <sys/resource.h>

extern char **environ;
int main(int argc,char *argv[]){//nie mozna tutaj envp bo wtedy po setowaniu sie nie zapisuje
	if(argc!=4){
		printf("Podaj nazwe pliku wsadowego, czas procesora(w sekundach), oraz rozmiar pamięci wirtualnej(w megabajtach)\n");
		return 1;
	}
	FILE * fp;
	char * line = NULL;
	size_t len = 0;
	ssize_t read;

	fp = fopen(argv[1], "r");
	if (fp == NULL){
		printf("nie udalo sie otworzyc pliku");
		return 1;
	}
	const char s[2] = " ";
	char* tokens[128];
	int i;
	while ((read = getline(&line, &len, fp)) != -1) {
		//printf("%s", line);
		i=0;
		tokens[0] = strtok(line,s);
		while( tokens[i] != NULL ){
			++i;
			tokens[i]=strtok(NULL, s);
		}
		if(!feof(fp)){//jesli nie koniec pliku to usuwamy entery
			tokens[i-1][strlen(tokens[i-1])-1]=0;
		}
		if(line[0]=='#'){//zmienna srododowiskowa
			tokens[0]=tokens[0]+1;
			if(i==2){//przypisanie
				const int overwrite=1;//Dla overwrite != 0: bezwarunkowo ustawia nazwe zmiennej
				setenv(tokens[0], tokens[1], overwrite);
				if(getenv(tokens[0])!=NULL && strcmp(tokens[1],getenv(tokens[0]))==0){
					printf("Ustawiono wartosc zmiennej srodowiskowej %s na %s\n",tokens[0],getenv(tokens[0]));
				}
				else{
					printf("Nie udalo sie zmienic wartosci zmiennej srodowiskowej %s\n",tokens[0]);
					return 1;
				}
			}
			else if(i==1){//usuniecie
				unsetenv(tokens[0]);
				if(getenv(tokens[0])!=NULL){
					printf("Nie udalo sie usunac zmiennej srodowiskowej %s\n",tokens[0]);
					return 1;
				}
				else{
					printf("Usunieto zmienna srodowiskowa %s\n",tokens[0]);
				}
			}
			else{
				printf("Zbyt duza liczba argumentow dla przypisania lub usuniecia zmiennej srodowiskowej");
				return 1;
			}
		}
		else if(i>0){//program do wykonania
			if(fork()==0){//potomny
				//nakladanie ograniczen
				struct rlimit timeLim;
				struct rlimit virMemLim;
				timeLim.rlim_cur = timeLim.rlim_max = atoi(argv[2]);//czas procesora
				virMemLim.rlim_cur = virMemLim.rlim_max = 1024*1024*atoi(argv[3]);//pamiec w MB
				setrlimit(RLIMIT_AS, &virMemLim);
				setrlimit(RLIMIT_CPU, &timeLim);
				execvpe(tokens[0], tokens,environ);
				perror("execvpe"); /* execve() only returns on error */
				return 1;
			}
			else{//rodzic
				int result;
				struct rusage childUsePrev;
				getrusage(RUSAGE_CHILDREN, &childUsePrev);
				wait(&result);
				if(result!=0){
					printf("Polecenie %s zakonczylo sie bledem zwracajac: %d\n",line,result);
					return 1;
				}
				struct rusage childUse;
				getrusage(RUSAGE_CHILDREN, &childUse);
				// user time: (long)childUse.ru_utime.tv_sec
				// system time: (long)childUse.ru_stime.tv_sec
				// maximum resident set size: childUse.ru_maxrss
				printf("Polcenie %s zajelo %ld KB pamieci, %ld mikrosek sys, %ld mikrosec uzyt\n",
					   line,childUse.ru_maxrss-childUsePrev.ru_maxrss,
					   (long)(1000000*(childUse.ru_stime.tv_sec-childUsePrev.ru_stime.tv_sec)+
							  childUse.ru_stime.tv_usec-childUsePrev.ru_stime.tv_usec),
					   (long)(1000000*(childUse.ru_utime.tv_sec-childUsePrev.ru_utime.tv_sec)+
							  childUse.ru_utime.tv_usec-childUsePrev.ru_utime.tv_usec)
					  );
			}
		}
	}

	fclose(fp);
	if (line)
		free(line);
	return 0;
}