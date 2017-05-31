#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <wait.h>
#include <pthread.h>
#include <string.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define RECORDSIZE 1024

int fileDesc=-1;
const char* fileName;
const char* searchWord;
volatile int threadsAmount;
volatile int recordsToRead;
volatile int lastPortionSize;
volatile int portionsAmount;
int* doneFilePortions=NULL;
pthread_t* threadIds;
int isMutexInitialized=0;
pthread_mutex_t mutex;

long gettid() {
    return syscall(SYS_gettid);
}

int binaryBufToInt(char* buf){//returns int at the start of buf which is stored in bytes so it needs to be converted
    int retVal=0;
    for(int i=0;i<sizeof(int);i++){
        //printf("retVal=%d\n",retVal);
        retVal+=(unsigned char)buf[i]<<(8*i);
    }
    return retVal;
}

int findWordInRecord(char* recordBuf,const char* word){//returns id of word or -1 if didnt found
    if(strstr(recordBuf+sizeof(int),word) != NULL){
        return binaryBufToInt(recordBuf);
    }
    return -1;
}

int threadMutexCheck(int portionNum,int threadNum){
    if(pthread_mutex_lock(&mutex)!=0){
        printf("error mutex lock at thread %d\n",threadNum);
        exit(1);
    }
    if(doneFilePortions[portionNum]!=0){//when already done or currently being done
        //printf("thread %d checked that %d portions is done\n",threadNum,portionNum);
        pthread_mutex_unlock(&mutex);
        return 1;
    }
    else{
        doneFilePortions[portionNum]=1;//currently being done
        //printf("thread %d is doing %d portion\n",threadNum,portionNum);
    }
    pthread_mutex_unlock(&mutex);
    return 0;
}

void threadRecordRead(char* buf, int portionNum, int recordsAmount,int threadNum){
    for(int j=0;j<recordsAmount;j++){
        if(pread(fileDesc, buf, RECORDSIZE, RECORDSIZE*(portionNum*recordsToRead+j))!=RECORDSIZE){
            printf("file might have been changed during working of programm\n");
            exit(1);
        }
        int found=findWordInRecord(buf,searchWord);
        if(found!=-1){
            if(pthread_mutex_lock(&mutex)!=0){
                printf("error mutex lock at thread %d\n",threadNum);
                exit(1);
            }
            printf("%d thread with system id %li found word \"%s\" at record with id %d\n",threadNum,gettid(),searchWord,found);
            for(int i=0;i<threadsAmount;i++){
                if(i!=threadNum){
                    if(threadIds[i]==-1){
                        printf("%d thread already exited\n",i);
                        continue;
                    }
                    if(pthread_cancel(threadIds[i])!=0){
                        //printf("%d errored on canceling: %d\n",threadNum,i);
                    }
                    //printf("%d canceled: %d\n",threadNum,i);
                }
            }
            pthread_mutex_unlock(&mutex);
            pthread_exit(0);
        }
    }
    doneFilePortions[portionNum]=2;
}

void* threadFunc(void* arg){
    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE,NULL);
    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS,NULL);
    int myNum=*(int*)arg;
    char buf[RECORDSIZE];
    //printf("portions amount %d\n",portionsAmount);
    for(int i=0;i<portionsAmount-1;i++){//-1 cause last is by itself
        if(threadMutexCheck(i,myNum)==1){
            continue;
        }
        threadRecordRead(buf,i,recordsToRead,myNum);
    }
    if(threadMutexCheck(portionsAmount-1,myNum)!=1){
        threadRecordRead(buf,portionsAmount-1,lastPortionSize,myNum);
    }
	return NULL;
}

void exitFunc(void){
    printf("exiting\n");
    if(fileDesc!=-1){
        close(fileDesc);
    }
    if(isMutexInitialized!=0){
        pthread_mutex_destroy(&mutex);
    }
    if(doneFilePortions!=NULL){
        free(doneFilePortions);
    }
    if(threadIds!=NULL){
        free(threadIds);
    }
}

void sigIntHdlr(int signum){
    printf("got sigint\n");
    exit(2);
}

int main(int argc, char **argv){
    //searching for searchWord in fileName by portions with threads
    if(argc<5){
        printf("four arguments: threads amount, file name, amount of records \
read by thread in a single operation and word to search\n");
        exit(1);
    }
    //init values
    threadsAmount=atoi(argv[1]);
    fileName=argv[2];
    recordsToRead=atoi(argv[3]);
    searchWord=argv[4];
    //making file with searchWord based random atribute texts instead of searching with threads
    if(argc==6){
        printf("do you want to overwrite %s?(y=yes)\n",fileName);
        char replyFromUser;
        fflush(stdout);
        replyFromUser=getchar();
        if(replyFromUser!='y'){
            printf("file unchanged\n");
            return 0;
        }
        FILE *fStream=fopen(fileName, "w+");
        const int bufSize=RECORDSIZE-sizeof(int);
        char buf[bufSize];
        for(int i=0;i<bufSize-1;i++){
            buf[i]=0;
        }
        buf[bufSize-1]='\n';
        strcpy(buf,searchWord);
        for(int i=0;i<recordsToRead;i++){
            for(int j=0;j<strlen(searchWord);j++){
                buf[j]=(buf[j]+j)%26+65;
            }
            fwrite(&i, sizeof(int), 1, fStream);
            fwrite(buf, sizeof(char), bufSize, fStream);
        }
        if(fStream!=0){
            fclose(fStream);
            printf("file changed\n");
        }
        return 0;
    }
    //exit func init and sig int mask
    atexit(exitFunc);
    signal(SIGINT,sigIntHdlr);
    //open file
    fileDesc=open(fileName, O_RDONLY, S_IRUSR);
    if(fileDesc==-1){
        printf("error with opening file %s\n",fileName);
        return 1;
    }
    //mutex init
    if(pthread_mutex_init(&mutex,NULL)==0){
        isMutexInitialized=1;
    }
    else{
        printf("error initializing mutex\n");
        return 1;
    }
    //filePortionsInit can be set to some maximum value instead of searching amount of portions
    char buf[RECORDSIZE];
    int offset=0;
    while(pread(fileDesc, buf, RECORDSIZE, offset)==RECORDSIZE){
        ++portionsAmount;
        offset+=RECORDSIZE;
    }
    lastPortionSize=portionsAmount%recordsToRead;
    portionsAmount=(int)(portionsAmount/recordsToRead)+1;
    
    doneFilePortions=calloc(sizeof(int),portionsAmount);//init doneFilePortions to zeros
    //threads init
    threadIds=calloc(sizeof(pthread_t),threadsAmount);
    if(threadIds==NULL){
        printf("error on calloc\n");
        return 1;
    }
    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE,NULL);
    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS,NULL);
    int threadNum[threadsAmount];
    if(pthread_mutex_lock(&mutex)!=0){
        printf("error mutex lock at main thread\n");
        return 1;
    }
	for(int i=0;i<threadsAmount;i++){
        threadNum[i]=i;
		if(pthread_create(&threadIds[i],NULL,&threadFunc,threadNum+i)!=0){
            printf("error on creating %d thread\n",i);
            return 1;
        }
        //printf("threadIds[%d]=%li\n",i,(long)threadIds[i]);
	}
    pthread_mutex_unlock(&mutex);
    //waiting for closing of threads
    for(int i=0;i<threadsAmount;i++) {
        //printf("threadIds[%d]=%li\n",i,(long)threadIds[i]);
        if(pthread_join(threadIds[i],NULL)) {
            return 1;
        }
    }
	return 0;
}