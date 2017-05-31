
#include <time.h>
#include <sys/times.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>

void start_clock(void);
void end_clock(char *msg);

static clock_t st_time;
static clock_t en_time;
static struct tms st_cpu;
static struct tms en_cpu;

void
start_clock()
{
    st_time = times(&st_cpu);
}

/* This example assumes that the result of each subtraction
   is within the range of values that can be represented in
   an integer type. */
void
end_clock(char *msg)
{
    en_time = times(&en_cpu);

    fputs(msg,stdout);
	fputs("\n",stdout);
    printf("Sys Time: %jd ms, User Time %jd ms\n",
        (en_time - st_time),
        (en_cpu.tms_utime - st_cpu.tms_utime));
}
static char *rand_string(char *str, size_t size)
{
    const char charset[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJK...";
    if (size) {
        --size;
        for (size_t n = 0; n < size; n++) {
            int key = rand() % (int) (sizeof charset - 1);
            str[n] = charset[key];
        }
        str[size] = '\0';
    }
    return str;
}
int main(int argc,char *argv[]){
	srand(time(NULL));
	// start timer
	start_clock();
	
	//./program sys shuffle datafile 100 512 
	//6 argumentow musi byc w argc
	if(argc!=6){
		printf("Musi byc sposob, co, nazwapliku, ile linii, ilosc bajtow w lini\n");
		return 1;
	}
	int isSys=0;//lib
	if(strcmp(argv[1],"sys")==0){
		isSys=1;
	}
	int whatOp=0;//shuffle
	if(strcmp(argv[2],"shuffle")!=0){
		if(strcmp(argv[2],"sort")==0){
			whatOp=1;	
		}
		else{//generate
			whatOp=2;
		}
	}
	char* fileName=argv[3];
	int fileLen=atoi(argv[4]);
	int recordSize=atoi(argv[5]);
	//printf("%d ,  %d , %s , %d , %d\n",isSys,whatOp,fileName,fileLen,recordSize);
	//Pojedynczy rekord będzie tablicą bajtów o stałej wielkości. Nazwa pliku, wielkość oraz ilość rekordów stanowić będą argumenty wywołania programu.
	int bytesWrote,bytesTook;
	if(whatOp ==2){//sys or lib
		int fileDesc = open(fileName, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR|S_IWUSR);
		int randDesc;
		if(isSys==1){
			randDesc = open("/dev/random", O_RDONLY);
		}
		unsigned char *buf = malloc((2+recordSize)*sizeof(unsigned char));
		buf[recordSize]='\n';
		for(int i=0;i<fileLen;i++){
			if(isSys==1){
				bytesTook = (int)(read(randDesc,buf,recordSize));
				if(bytesTook!=recordSize){
					printf("error taking bytes from rand\n");
					return 1;
				}
			}
			else{
				buf=rand_string(buf,recordSize+2);	
			}
			buf[recordSize]='\n';
			bytesWrote = (int)(write(fileDesc,buf,recordSize+1));
			if(bytesWrote!=recordSize+1){
				printf("error saving bytes to %s\n",fileName);
				return 1;
			}
			//printf("%d took, %d wrote\n",bytesTook,bytesWrote);
		}
	}
	else if(whatOp == 0){//shuffle
		if(isSys==1){//sys
			int fileDesc = open(fileName, O_RDWR, S_IRUSR|S_IWUSR);
			unsigned char* buf1 = malloc(sizeof(unsigned char)*recordSize+1);
			unsigned char* buf2 = malloc(sizeof(unsigned char)*recordSize+1);
			unsigned char* tmpBuf;
			for(int i=0;i<fileLen;i++){
				lseek(fileDesc,i*(recordSize+1),SEEK_SET);
				bytesTook = (int)(read(fileDesc,buf1,recordSize));
				if(bytesTook!=recordSize){
					printf("error taking bytes buf1\n");
					return 1;
				}
				for(int k=i+1;k<fileLen;k++){
					int j=(fileLen - k)*((double)rand()/RAND_MAX) + k;
					lseek(fileDesc,j*(recordSize+1),SEEK_SET);
					bytesTook = (int)(read(fileDesc,buf2,recordSize));
					if(bytesTook!=recordSize){
						printf("error taking bytes buf2\n");
						return 1;
					}
					lseek(fileDesc,j*(recordSize+1),SEEK_SET);
					tmpBuf=buf1;
					bytesWrote = (int)(write(fileDesc,buf1,recordSize));
					if(bytesWrote!=recordSize){
						printf("error saving buf1 bytes to %s\n",fileName);
						return 1;
					}
					buf1=buf2;
					buf2=tmpBuf;//przepinamy wskazniki
				}
				lseek(fileDesc,i*(recordSize+1),SEEK_SET);
				bytesWrote = (int)(write(fileDesc,buf1,recordSize));//buf1 bo byly przepiete
				if(bytesWrote!=recordSize){
					printf("error saving buf1 bytes to %s\n",fileName);
					return 1;
				}
			}
		}
		else{//lib
			FILE *fStream=fopen(fileName, "r+");
			unsigned char* buf1 = malloc(sizeof(unsigned char)*recordSize+1);
			unsigned char* buf2 = malloc(sizeof(unsigned char)*recordSize+1);
			unsigned char* tmpBuf;
			for(int i=0;i<fileLen;i++){
				fseek(fStream,i*(recordSize+1),SEEK_SET);
				bytesTook = (int)(fread(buf1,1,recordSize,fStream));//ewentualnie zmiana 1<->recordSize
				if(bytesTook!=recordSize){
					printf("error taking bytes buf1\n");
					return 1;
				}
				for(int k=i+1;k<fileLen;k++){
					int j=(fileLen - k)*((double)rand()/RAND_MAX) + k;
					fseek(fStream,j*(recordSize+1),SEEK_SET);
					bytesTook = (int)(fread(buf2,1,recordSize,fStream));
					if(bytesTook!=recordSize){
						printf("error taking bytes buf2\n");
						return 1;
					}
					fseek(fStream,j*(recordSize+1),SEEK_SET);
					tmpBuf=buf1;
					bytesWrote = (int)(fwrite(buf1,1,recordSize,fStream));
					if(bytesWrote!=recordSize){
						printf("error saving buf1 bytes to %s\n",fileName);
						return 1;
					}
					buf1=buf2;
					buf2=tmpBuf;//przepinamy wskazniki
				}
				fseek(fStream,i*(recordSize+1),SEEK_SET);
				bytesWrote = (int)(fwrite(buf1,1,recordSize,fStream));//buf1 bo byly przepiete
				if(bytesWrote!=recordSize){
					printf("error saving buf1 bytes to %s\n",fileName);
					return 1;
				}
			}
			fclose(fStream);
		}
	}
	/*
	procedure bubbleSort( A : lista elementów do posortowania )
	  n = liczba_elementów(A)
	   do
		for (i = 0; i < n-1; i++) do:
		  if A[i] > A[i+1] then
			swap(A[i], A[i+1])
		  end if
		end for
		n = n-1
	  while n > 1
	end procedure
	*/
	else{//sort
		if(isSys==1){//sys
			int fileDesc = open(fileName, O_RDWR, S_IRUSR|S_IWUSR);
			unsigned char* buf1 = malloc(sizeof(unsigned char)*recordSize+1);
			unsigned char* buf2 = malloc(sizeof(unsigned char)*recordSize+1);
			unsigned char* tmpBuf;
			for(int i=fileLen;i>0;i--){
				//lseek(fileDesc,i*(recordSize+1),SEEK_SET);//bierzemy i-ty
				lseek(fileDesc,0,SEEK_SET);
				bytesTook = (int)(read(fileDesc,buf1,recordSize));
				if(bytesTook!=recordSize){
					printf("error taking bytes buf1\n");
					return 1;
				}
				//lseek(fileDesc,1,SEEK_CUR);//bo jeszcze \n
				for(int j=1;j<i;j++){
					//lseek(fileDesc,(recordSize+1),SEEK_CUR);//bo w babelkowym idziemy o 1 element do przodu tylko
					lseek(fileDesc,j*(recordSize+1),SEEK_SET);
					bytesTook = (int)(read(fileDesc,buf2,recordSize));
					if(bytesTook!=recordSize){
						printf("error taking bytes buf2\n");
						return 1;
					}
					//lseek(fileDesc,1,SEEK_CUR);//bo jeszcze \n
					//teraz w buf1 musi byc ten z prawej
					if(buf1[0]>buf2[0]){
						//cofamy sie o dwa i zapisujemy w odwrotnej kolejnosci
						//lseek(fileDesc,-2*(recordSize+1),SEEK_CUR);
						lseek(fileDesc,(j-1)*(recordSize+1),SEEK_SET);
						//zapisujemy buf2
						bytesWrote = (int)(write(fileDesc,buf2,recordSize));
						if(bytesWrote!=recordSize){
							printf("error saving buf2 bytes to %s\n",fileName);
							return 1;
						}
						//lseek(fileDesc,1,SEEK_CUR);
						//zapisujemy buf1
						lseek(fileDesc,j*(recordSize+1),SEEK_SET);
						bytesWrote = (int)(write(fileDesc,buf1,recordSize));
						if(bytesWrote!=recordSize){
							printf("error saving buf1 bytes to %s\n",fileName);
							return 1;
						}
						//lseek(fileDesc,1,SEEK_CUR);
						//jestesmy po dwoch elementach i w buf1 jest wiekszy wiec ok
					}
					else{
						tmpBuf=buf1;
						buf1=buf2;
						buf2=tmpBuf;
					}
				}
			}//dziala
		}
		else{//lib
			FILE *fStream=fopen(fileName, "r+");
			unsigned char* buf1 = malloc(sizeof(unsigned char)*recordSize+1);
			unsigned char* buf2 = malloc(sizeof(unsigned char)*recordSize+1);
			unsigned char* tmpBuf;
			for(int i=fileLen;i>0;i--){
				fseek(fStream,0,SEEK_SET);
				bytesTook = (int)(fread(buf1,1,recordSize,fStream));
				if(bytesTook!=recordSize){
					printf("error taking bytes buf1\n");
					return 1;
				}
				for(int j=1;j<i;j++){
					fseek(fStream,j*(recordSize+1),SEEK_SET);
					bytesTook = (int)(fread(buf2,1,recordSize,fStream));
					if(bytesTook!=recordSize){
						printf("error taking bytes buf2\n");
						return 1;
					}
					//teraz w buf1 musi byc ten z prawej
					if(buf1[0]>buf2[0]){
						//cofamy sie o dwa i zapisujemy w odwrotnej kolejnosci
						fseek(fStream,(j-1)*(recordSize+1),SEEK_SET);
						//zapisujemy buf2
						bytesWrote = (int)(fwrite(buf2,1,recordSize,fStream));
						if(bytesWrote!=recordSize){
							printf("error saving buf2 bytes to %s\n",fileName);
							return 1;
						}
						//zapisujemy buf1
						fseek(fStream,j*(recordSize+1),SEEK_SET);
						bytesWrote = (int)(fwrite(buf1,1,recordSize,fStream));
						if(bytesWrote!=recordSize){
							printf("error saving buf1 bytes to %s\n",fileName);
							return 1;
						}
						//jestesmy po dwoch elementach i w buf1 jest wiekszy wiec ok
					}
					else{
						tmpBuf=buf1;
						buf1=buf2;
						buf2=tmpBuf;
					}
				}
			}
		}
	}

	// stop timer
	end_clock(".");
	return 0;	
}