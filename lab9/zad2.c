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
volatile int volatile table[tableSize];
pthread_mutex_t mutex;
pthread_cond_t* conds;
int* taken;

pthread_t* writers;
pthread_t* readers;

void* writer(void* arg){
    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
    int writingCount;
    int index;
    int value;
    int qplace;
    while(1) {
        qplace = -1;
        int i;
        for (i = 0; i < writerAmount + readerAmount; i++) {
            if (taken[i] == 0) {
                qplace = i;
                taken[i] = 1;
                break;
            }
        }
        if (qplace == -1)continue;
        pthread_mutex_lock(&mutex);
        pthread_cond_wait(&conds[qplace], &mutex);
        taken[qplace] = 1;
        writingCount = rand() % ((int)(tableSize/2));
        for (i = 0; i < writingCount; i++) {
            index = rand() % tableSize;
            value = rand() % tableRange;
            table[index] = value;
            if(verbose) printf("Writer \"%ld\" wrote %d at index %d\n", pthread_self(), i, table[i]);
        }
        printf("Writer \"%ld\" has ended modyfing table\n", pthread_self());
        taken[qplace] = 0;
        pthread_cond_broadcast(&conds[qplace]);
        pthread_mutex_unlock(&mutex);
    }
    return NULL;
}

void* reader(void* arg){
    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
    int divisibleCount;
    int qplace;
    int div= *((int *)arg);
    while(1){
        qplace=-1;
        divisibleCount=0;
        int i;
        for(i=0; i<writerAmount+readerAmount; i++){
            if(taken[i]==0){
                qplace=i;
                taken[i]=1;
                break;
            }
        }
        if(qplace==-1)continue;
        pthread_mutex_lock(&mutex);
        pthread_cond_wait(&conds[qplace], &mutex);
        taken[qplace]=1;
        for(i=0; i<tableSize; i++){
            if(table[i]%div==0){
                divisibleCount++;
                if(verbose) printf("Reader \"%ld\" found %d at index %d\n", pthread_self(), table[i], i);
            }
        }
        printf("Reader \"%ld\" found %d nums divisible by %d\n", pthread_self(), divisibleCount, div);
        taken[qplace]=0;
        pthread_cond_signal(&conds[qplace]);
        pthread_mutex_unlock(&mutex);
    }
    return NULL;
}

void sighandler(int signo){
    for(int i=0; i<writerAmount; i++) pthread_cancel(writers[i]);
    for(int i=0; i<readerAmount; i++) pthread_cancel(readers[i]);
    //sem_close(block);
    for(int i=0; i<readerAmount+writerAmount; i++) pthread_cond_destroy(&conds[i]);
    pthread_mutex_destroy(&mutex);
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
    else verbose=1;
//    block=malloc(sizeof(sem_t));
    conds=calloc(writerAmount+readerAmount, sizeof(pthread_cond_t));
    taken=calloc(writerAmount+readerAmount, sizeof(int));
    for(int i=0; i<writerAmount+readerAmount; i++){
        pthread_cond_init(&conds[i], NULL);
        taken[i]=0;
    }
    taken = calloc(writerAmount+readerAmount, sizeof(int));
    if(argc>3 && strcmp(argv[3], "-i")==0) verbose=1;
    else verbose=0;
    pthread_mutex_init(&mutex, NULL);
    //mutex = PTHREAD_MUTEX_INITIALIZER;
    for(int i=0; i<writerAmount; i++){
        pthread_create(&writers[i], NULL, writer, NULL);
    }
    int div[readerAmount];
    for(int i=0; i<readerAmount; i++){
        div[i]=1+rand()%((int)((tableRange-1)/2));//cant be 0
        pthread_create(&readers[i], NULL, reader, (void*)&div[i]);
    }
    signal(SIGINT, sighandler);
    for(int rnr=0; 1; rnr=(rnr+1)%(writerAmount+readerAmount)){//inf loop
        if(taken[rnr]!=0){
            pthread_cond_signal(&conds[rnr]);
            sleep(1);
        }
    }
    return 0;
}
