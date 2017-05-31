#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <errno.h>
extern char **environ;
int main(int argc,char *argv[]){
	size_t len=0;
	char* line;
	char buf[128];
	const char s[2] = "|";
	const char s2[2] = " ";
	char* tokens[128];
	char* tokens2[128];
	int argSize;
	int argSize2;
	while (getline(&line, &len, stdin) != -1) {
		argSize=0;
		tokens[0] = strtok(line,s);
		while( tokens[argSize] != NULL ){
			++argSize;
			tokens[argSize]=strtok(NULL, s);
		}
		int pipefd[argSize][2];
		for(int i=0;i<argSize;i++){
			if(pipe(pipefd[i])!=0){
				printf("nie udalo sie stworzyc pipe %d\n",i);
				exit(1);
			}
		}
		for(int bigIter=0;bigIter<argSize;bigIter++){
			//printf("tokens[%d]=%s\n",bigIter,tokens[bigIter]);
			argSize2=0;
			tokens2[0] = strtok(tokens[bigIter],s2);
			while( tokens2[argSize2] != NULL ){
				++argSize2;
				tokens2[argSize2]=strtok(NULL, s2);
			}
			//printf("trying to use %s with args and size %d\n",tokens2[0],argSize2);
			tokens2[argSize2]=NULL;
			/*for(int i=0;;i++){
				if(tokens2[i]==NULL){
					break;
				}
				printf("tokens2[%d]=%s\n",i,tokens2[i]);
			}*/
			if(bigIter==argSize-1){//jesli ostatni
				tokens2[argSize2-1][strlen(tokens2[argSize2-1])-1]=0;
			}
			if(fork()==0){
				close(pipefd[bigIter][0]);
				if(bigIter!=0) dup2(pipefd[bigIter-1][0], STDIN_FILENO);//dostaje dane w fd1 a zwraca w fd2
				if(dup2(pipefd[bigIter][1],STDOUT_FILENO)==-1){
					printf("errno = %d\n\n",errno);
				}
				execvpe(tokens2[0],tokens2,environ);
				perror("execvpe");
				exit(1);
			}
			else{
				close(pipefd[bigIter][1]);
				if(bigIter!=0) close(pipefd[bigIter-1][0]);
				wait(NULL);//czekamy az sie skonczy 
				if(bigIter!=argSize-1){
				//	while(read(pipefd[bigIter+1][0],&buf,1)>0){//przepisujemy spowrotem na pipefd1
				//		write(pipefd[bigIter+2][1],&buf,1);
						//write(STDOUT_FILENO,"xdXDxd",1);
				//	}
				}
				else{//jesli ostatni to nic juz
					while(read(pipefd[bigIter][0],&buf,1)>0){
						write(STDOUT_FILENO,&buf,1);
					}
					write(STDOUT_FILENO,"\n",1);
				}
			}
		}
	}
	//execvp(argv[1],argv+1);
	/*
	dup2(pipefd[0], STDIN_FILENO);
	dup2(pipefd[1], STDOUT_FILENO);*/
	return 0;
}