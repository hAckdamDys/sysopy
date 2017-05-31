#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <wait.h>
#include <sys/stat.h>
#include <sys/shm.h>
#include <sys/msg.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <time.h>
#include <unistd.h>

#define BARBER 0
#define FIFO 1
#define CLIENT 2
#define CUT 3

int N;
int *shmTab=NULL;
int shmId=-1;
int semId=-1;

int fifo_is_empty(pid_t *tab) {
    return tab[1] == 0 ? 1 : 0;
}

pid_t fifo_pop(pid_t *tab) {
    if(fifo_is_empty(tab)) return -1;

    int i = tab[2];
    tab[2] = (tab[2] + 1 + 4 * ((tab[2] + 1) / tab[0])) % tab[0]; // looping jesli przekroczymy limit
    --tab[1];


    return tab[i];
}

int getTime() {
    struct timespec time;
    if (clock_gettime(CLOCK_MONOTONIC, &time) == -1){
		printf("problem z czasem\n");
	}
    return time.tv_nsec/1000;
}

void give_semaphore(int semId, unsigned short semIndex) {
    struct sembuf smbuf;
    smbuf.sem_num = semIndex;
    smbuf.sem_op = 1;
    smbuf.sem_flg = 0;
    if (semop(semId, &smbuf, 1) == -1){
        printf("error on give seemaphor\n");
		exit(1);
	}
}

void take_semaphore(int semId, unsigned short semIndex) {
    struct sembuf smbuf;
    smbuf.sem_num = semIndex;
    smbuf.sem_op = -1;
    smbuf.sem_flg = 0;
    if (semop(semId, &smbuf, 1) == -1){
        printf("error on take seemaphor\n");
		exit(1);
	}
}

void make_cut(int client_pid) {
    printf("Golimy podczas czasu: %d klienta %d\n", getTime(), client_pid);
    give_semaphore(semId,CUT);
    //kill(client_pid, SIGRTMIN);
    printf("Koniec golenia podczas czasu: %d klienta %d\n", getTime(), client_pid);
}


void cleanAll(){
    if (shmTab != NULL) {
        shmdt(shmTab);
    }
    if(shmId!=-1){
		shmctl(shmId,IPC_RMID,NULL);
    }
    if(semId != -1) {
        semctl(semId,0,IPC_RMID);
    }
	printf("zamykamy barbera\n");
}

void sigIntHdlr(int signum){
	printf("sigint\n");
	exit(0);
}

int main(int argc, char** argv) {
    if(argc != 2) {
        printf("podaj ilosc siedzen w poczekalni\n");
		return 1;
    }
	N = atoi(argv[1]);
    atexit(cleanAll);
	signal(SIGINT,sigIntHdlr);
	key_t key = ftok(".",'p');

    semId = semget(key, 4, IPC_CREAT | 0600);
    if(semId == -1) {
		printf("Error on semget\n");
	}

    if(semctl(semId,BARBER,SETVAL,0) == -1 ||
       semctl(semId,FIFO,SETVAL,1) ||
       semctl(semId,CLIENT,SETVAL,1) ||
       semctl(semId,CUT,SETVAL,0)){
		printf("Error while initializing the semaphores value\n");
	}

    shmId = shmget(key, (N+4)*sizeof(pid_t), IPC_CREAT | 0600);
    if(shmId == -1)	{
		printf("Error while creating shared memory\n");
		exit(1);
	}

    shmTab = shmat(shmId, NULL, 0);
    if(shmTab == (int *) -1) {
		printf("Error while getting address of shared memory\n");
		exit(1);
	}

    shmTab[0] = N+4; //dlugosc calej kolejki
    shmTab[1] = 0; // ile klientow
    shmTab[2] = 4; // index
	int client;
	printf("Zasypiam podczas: %zu \n", getTime());
	while(1){//koniec przy sigincie tylko
		
        take_semaphore(semId,BARBER); // sleep
        take_semaphore(semId,FIFO); // waiting for fifo, to take client
        client = shmTab[3];//ile jest na krzesle
        give_semaphore(semId,FIFO);
        make_cut(client);

        while(1){
            take_semaphore(semId,FIFO);
            client = fifo_pop(shmTab);
            if(client!=-1){
                make_cut(client);
                give_semaphore(semId,FIFO);
            }
            else{
                printf("Zasypiam podczas: %zu \n", getTime());
                take_semaphore(semId,BARBER);
                give_semaphore(semId,FIFO);
                break;
            }
        }
	}
	
	
	
	return 0;
}