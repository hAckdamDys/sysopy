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

#define EXIT_FAILURE 1
#define EXIT_SUCCESS 0



int N,S;
int semId;
int *shmTab;
int actualCuts = 0;

int fifo_is_full(pid_t *tab) {
    return tab[0]-4 == tab[1] ? 1 : 0;
}

int fifo_push(pid_t *tab, pid_t client_pid) {
    if(fifo_is_full(tab)) return -1;
    //printf("I'm CLIENT: %d, the fifo is not full :)\n",client_pid);
    int i = ((tab[1] + tab[2]) % tab[0]) + 4 * ((tab[1]+tab[2]) / tab[0]); // magic index :) (nah, just looping back if exceeded the range)
    //printf("I'm CLIENT: %d, The value of i: %d\n",client_pid,i);
    tab[i] = client_pid;

    ++tab[1]; // incrementing actual clients

    return 0;
}

int getTime() {
    struct timespec time;
    if (clock_gettime(CLOCK_MONOTONIC, &time) == -1){
		printf("problem z czasem\n");
	}
    return time.tv_nsec/1000;
}

void exit_program(int status, char *message) {
    status == EXIT_SUCCESS? printf("%s\n",message) : perror(message);
    exit(status);
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

int try_to_seat() {
    int status;
    take_semaphore(semId,FIFO);
    int barber_semval = semctl(semId,BARBER,GETVAL);
    if(barber_semval == -1) {
        exit_program(EXIT_FAILURE, "Barber semaphore checking error");
    }

    if(barber_semval == 0) {    // if barber sleeping
        printf("%d, %d CLIENT: Barber is sleeping. Let's wake him up.\n", getTime(), getpid());
        give_semaphore(semId,BARBER); // wake him up
        give_semaphore(semId,BARBER); // WAKE HIM MORE UP (indicates that barber is working - let to know it to the others)
        shmTab[3]=getpid(); // sit on chair
        status = 0;
        // and give him the fifo semaphore
    } else { // if barber doing something try to seat in the waiting room
        if(fifo_push(shmTab,getpid()) == -1) { // if it is full
            printf("%d, %d CLIENT: Barber is full. Going away.\n", getTime(), getpid());
            status = -1;
        } else {
            printf("%d, %d CLIENT: Barber is busy. Taking seat in waiting room. \n", getTime(), getpid());
            status = 0;
        }
    }

    give_semaphore(semId,FIFO); // give the semaphore to someone else
    return status;
}

int visit_barber() {

    while(actualCuts < S) {
        //take_semaphore(semId,CLIENT);    // synchronizing clients' access to barber shop - may be not used
        int res = try_to_seat();
        //give_semaphore(semId,CLIENT);
        if(res == 0) {
            take_semaphore(semId,CUT);
            //sigsuspend(&mask);
            ++actualCuts;
            printf("%d, %d CLIENT: Got pretty cut.\n", getTime(), getpid());
        }
    }
    if(shmdt(shmTab) == -1)
        exit_program(EXIT_FAILURE,"Error while detaching from shared memory");
    return 0;
}

void sigIntHdlr(int signum){
	printf("sigint\n");
	exit(0);
}


void init_res() {
    key_t key = ftok(".",'p');
    int shm_id = shmget(key,0,0);
    if(shm_id == -1)
        exit_program(EXIT_FAILURE, "Error while getting shared memory");

    shmTab = shmat(shm_id,NULL,0);
    if(shmTab == (int *) -1)
        exit_program(EXIT_FAILURE, "Error while getting address of shared memory");

    semId = semget(key,0,0);
    if(semId == -1)
        exit_program(EXIT_FAILURE,"Error while getting semaphores");
}

int main(int argc, char **argv) { // N - number of clients to create and S - number of required cuts
    if(argc != 3) {
        exit_program(EXIT_FAILURE, "Bad number of arguments! Pass the number of clients to create and the number of required cuts");
    }

    N = atoi(argv[1]); S = atoi(argv[2]);

    signal(SIGINT,sigIntHdlr);
    
    init_res();

    for(int i = 0; i < N; ++i) {
        if(fork() == 0) { // don't let children create their own children
            visit_barber() == -1 ? _exit(EXIT_FAILURE) : _exit(EXIT_SUCCESS);
        }
    }

    int clients_served = 0;
    int status;
    while(clients_served < N) {
        wait(&status);
        ++clients_served;
        if(status == EXIT_SUCCESS) ++clients_served;
        else exit_program(EXIT_SUCCESS, "Child failed to execute properly"); // success, because it's not fault of parent process
    }

    return 0;
}