#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <limits.h>//PIPE_BUF
#include <time.h>
int main(int argc, char **argv){
	if(argc!=3){
		printf("slave przyjmuje 2 argi: nazwa potoku, liczba calkowita R obraz obszaru\n");
		exit(1);
	}
	char* PipeName=argv[1];
	int R=atoi(argv[2]);
	int Tt[R][R];
	for(int i=0;i<R;i++){
		for(int j=0;j<R;j++){
			Tt[i][j]=0;
		}	
	}
	mkfifo(PipeName,0666);
	char buf[PIPE_BUF];
	int fd=open(PipeName,O_RDONLY);
	double nReal,nComp;
	int nIter;
	int length=0;
	int readLen;
	char strTmp[64];
	//N punktow  D = { c: -2 < Re(c) < 1, -1 < Im(c) < 1 }
	const int rangeRe = 3;
	int rPos,iPos;
	const int rangeIm = 2;
	char strPrev[64];
	while((readLen=read(fd,&buf,PIPE_BUF))>0){
		//write(STDOUT_FILENO,&buf,readLen);
		//printf("^normal and our\n");
		length=0;
		while(length<readLen){
			sscanf (buf+length, "%64[^\n]\n", strTmp);
			int isGoodArgNum=sscanf (buf+length, "%lf %lf %d\n", &nReal,&nComp,&nIter);
			length+=strlen(strTmp)+1;
			//printf("%lf %lf %d\n", nReal,nComp,nIter);
			//printf("pozycja R:%lf:%d, pozycja Im:%lf:%d, war:%d\n",nReal,(int)(((nReal+2.0)*R)/rangeRe),nComp,(int)(((nComp+1.0)*R)/rangeIm),nIter);
			rPos=(int)(((nReal+2.0)*R)/rangeRe);
			iPos=(int)(((nComp+1.0)*R)/rangeIm);
			if(rPos<R && iPos<R && rPos>=0 && iPos>=0) {
				if(isGoodArgNum==3){
					Tt[rPos][iPos]=nIter;
				}
				else{//jakis bardziej skomplikowany bug nie do ogarniecia
				}
			}
			else{
					//printf("pozycja R:%lf:%d, pozycja Im:%lf:%d, war:%d\n",nReal,(int)(((nReal+2.0)*R)/rangeRe),nComp,(int)(((nComp+1.0)*R)/rangeIm),nIter);
					//printf("s:%s\n",strTmp);
					//printf("sPrev:%s\n",strPrev);
					strPrev[strlen(strPrev)-1]=0;
					strcat (strPrev, strTmp);
					//printf("snew:%s\n",strPrev);
					int isGoodArgNum=sscanf (strPrev, "%lf %lf %d\n", &nReal,&nComp,&nIter);
					rPos=(int)(((nReal+2.0)*R)/rangeRe);
					iPos=(int)(((nComp+1.0)*R)/rangeIm);
					if(isGoodArgNum==3 && rPos<R && iPos<R && rPos>=0 && iPos>=0){
						//printf("FIXED!\n");
						Tt[rPos][iPos]=nIter;
					}
			}
			strcpy(strPrev,strTmp);
		}
	}
	close(fd);
	unlink(PipeName);
	FILE* dataF=fopen("data","w");
	for(int i=0;i<R;i++){
		for(int j=0;j<R;j++){
			fprintf(dataF,"%d %d %d\n",i,j,Tt[i][j]);
		}
	}
	fclose(dataF);
	FILE* gnuFp=popen("gnuplot","w");
	fprintf(gnuFp,"set view map\nset xrange [0:%d]\nset yrange [0:%d]\nplot 'data' with image\n",R,R);
	fflush(gnuFp);
	getc(stdin);
	pclose(gnuFp);
	exit(0);
}