#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <limits.h>//PIPE_BUF
#include <complex.h>
#include <time.h>
int iters(double complex num,int K){
	double complex z=0;//z_0
	for(int i=0;i<K;i++){
		if(cabs(z)>2){
			return i;
		}
		z=z*z+num;
	}
	return K;
}
int main(int argc, char **argv){
	if(argc!=4){
		printf("slave przyjmuje 3 argi: nazwa potoku, N punktow, K iteracji\n");
		exit(1);
	}
	srand(time(NULL) ^ (getpid()<<16));
	char* PipeName=argv[1];
	int N=atoi(argv[2]);
	int K=atoi(argv[3]);
	char buf[PIPE_BUF];
	int fd=open(PipeName,O_WRONLY);
	//read(fd,buf,PIPE_BUF);
	//printf("%d\n",PIPE_BUF);
	//N punktow  D = { c: -2 < Re(c) < 1, -1 < Im(c) < 1 }
	int length = 0;
	for(int i=0;i<N;i++){
		double complex num=-2 + (rand() / ( RAND_MAX / (3.0) ) ) + (-1 + (rand() / ( RAND_MAX / (2.0) ) ))*I;
		//printf("%f %f %d\n",creal(num),cimag(num),iters(num,K));
		length += snprintf(buf+length, PIPE_BUF, "%f %f %d\n", creal(num),cimag(num),iters(num,K));
		if(length>(PIPE_BUF-100)){
			//printf("%s",buf);
			write(fd,buf,length);
			length=0;
		}
	}
	if(length>0){
		write(fd,buf,length);
		//printf("%s",buf);
		//printf("%s",buf);
	}
	close(fd);
	exit(0);
}