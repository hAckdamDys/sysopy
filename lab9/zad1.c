#include<stdio.h>
#include<semaphore.h>
#include<pthread.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<signal.h>
#include<errno.h>

#define tableSize 8
#define tableRange 16

int verbose=0;

int writerAmount;
int readerAmount;
int currentReadersAmount=0;

volatile int volatile table[tableSize];
sem_t writeSemafor;
sem_t readSemafor;
pthread_t* writers;
pthread_t* readers;

void* writer(void* arg){
    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
    int cnt;
    int index;
    int value;
    while(1){
        //sleep(1);
        sem_wait(&writeSemafor);
        cnt = rand() % ((int)(tableSize/2));
        for(int i=0; i<cnt; i++){
            index=rand()%tableSize;
            value=rand()%tableRange;
            table[index]=value;
            if(verbose) printf("Writer \"%ld\" wrote %d at index %d\n", pthread_self(), i, table[i]);
        }
        printf("Writer \"%ld\" has ended modyfing table\n", pthread_self());
        sem_post(&writeSemafor);
        //sleep(rand()%2);
    }
    return NULL;
}

void* reader(void* arg){
    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
    int divisibleCount;
    int div= *((int *)arg);
    while(1){
        //sleep(1);
        sem_wait(&readSemafor);
	if(1==++currentReadersAmount){
		sem_wait(&writeSemafor);
	}
        sem_post(&readSemafor);

        divisibleCount=0;
        for(int i=0; i<tableSize; i++){
            if(table[i]%div==0){
                divisibleCount++;
                if(verbose) printf("Reader \"%ld\" found %d at index %d\n", pthread_self(), table[i], i);
            }
        }
        printf("Reader \"%ld\" found %d nums divisible by %d\n", pthread_self(), divisibleCount, div);
        sem_wait(&readSemafor);
	if(0==--currentReadersAmount){
		sem_post(&writeSemafor);
	}
        sem_post(&readSemafor);
        sleep(rand()%5);
    }
    return NULL;
}

void sighandler(int signo){
    for(int i=0; i<writerAmount; i++) pthread_cancel(writers[i]);
    for(int i=0; i<readerAmount; i++) pthread_cancel(readers[i]);
    sem_close(&readSemafor);
    sem_close(&writeSemafor);
    exit(signo);
}

int main(int argc, char* argv[]){
    srand(time(NULL));
    if(argc<3){
        printf("writerAmount,readerAmount, needed\n");
        return 1;
    }

    writerAmount = atoi(argv[1]);
//    writerAmount=3;
//    readerAmount=3;
    readerAmount= atoi(argv[2]);
    writers=calloc(writerAmount, sizeof(pthread_t));
    readers=calloc(readerAmount, sizeof(pthread_t));
    if(argc>3 && strcmp(argv[3], "-i")==0) verbose=1;
    else verbose=0;
//    tableSemafor=malloc(sizeof(sem_t));
    sem_init(&writeSemafor, 1, 1);
    sem_init(&readSemafor, 1, 1);
    sem_wait(&writeSemafor);
    sem_wait(&readSemafor);
    for(int i=0; i<writerAmount; i++){
        pthread_create(&writers[i], NULL, writer, NULL);
    }
    int div[readerAmount];
    for(int i=0; i<readerAmount; i++){
        div[i]=1+rand()%((int)((tableRange-1)/2));//cant be 0
        pthread_create(&readers[i], NULL, reader, (void*)&div[i]);
    }
    signal(SIGINT, sighandler);
    if(sem_post(&readSemafor)==-1){
        printf("error on sem_post\n");
    }
    if(sem_post(&writeSemafor)==-1){
        printf("error on sem_post\n");
    }
    for(int i=0; i<writerAmount; i++){
        pthread_join(writers[i], NULL);
    }
    for(int i=0; i<readerAmount; i++){
        pthread_join(readers[i], NULL);
    }
    return 0;
}
