#include "communication.h"

int N;
pid_t *shmTab;
sem_t* semTab[3];

void free_res() {
    if (munmap(shmTab, (N+4)*sizeof(int)) == -1) {
        perror("Error while detaching shared memory");
    }
    if(shm_unlink(MEMORY_STR) == -1) {
        perror("Error while deleting memory segment");
    }
    if(sem_unlink(BARBER_STR) == -1 || sem_unlink(FIFO_STR) == -1 || sem_unlink(CUT_STR) == -1) {
        perror("Error while deleting semaphores");
    }
}

void exit_handler() {
    free_res();
}

void sigint_handler(int signum) {
    exit_program(EXIT_SUCCESS,"Sigint received.");
}

int set_sigint_handling() {
    struct sigaction sig;
    sig.sa_flags = 0;
    sigset_t mask;
    if(sigemptyset(&mask) == -1) return -1;
    sig.sa_mask = mask;
    sig.sa_handler = sigint_handler;
    return sigaction(SIGINT, &sig, NULL);
}

void make_cut(int client_pid) {
    printf("Poczatek golenia podczas %zu, %d\n", getTime(), client_pid);
    sem_post(semTab[CUT]);
    printf("Koniec golenia podczas: %zu klienta %d\n", getTime(), client_pid);
}

void init_res() {
    semTab[BARBER] = sem_open(BARBER_STR, O_CREAT | O_RDWR | O_EXCL, 0600, 0);
    semTab[FIFO] = sem_open(FIFO_STR, O_CREAT | O_RDWR | O_EXCL, 0600, 1);
    semTab[CUT] = sem_open(CUT_STR, O_CREAT | O_RDWR | O_EXCL, 0600, 0);

    if(semTab[BARBER] == SEM_FAILED || semTab[FIFO] == SEM_FAILED || semTab[CUT] == SEM_FAILED) {
        exit_program(EXIT_FAILURE, "Semaphore creating error");
    }

    int shm_id = shm_open(MEMORY_STR, O_CREAT | O_RDWR | O_EXCL, 0600);
    if(shm_id == -1) {
        exit_program(EXIT_FAILURE, "Error while creating shared memory");
    }
    if(ftruncate(shm_id, (N+4)*sizeof(pid_t)) == -1) {
        exit_program(EXIT_FAILURE, "Error while truncating shared memory");
    }
    shmTab = mmap(NULL, (N+4)*sizeof(pid_t), PROT_READ | PROT_WRITE, MAP_SHARED, shm_id, 0);
    if(shmTab == (void*)-1) {
        exit_program(EXIT_FAILURE, "Error while getting address of shared memory");
    }

    fifo_init(shmTab,N);
}

int main(int argc, char** argv) { // N - amount of chairs in waiting room
    if(argc != 2) {
        printf("podaj ilosc siedzen w poczekalni\n");
		exit(1);
    }
    atexit(exit_handler);
    set_sigint_handling();

    N = atoi(argv[1]);

    if(set_sigint_handling() == -1) {
        exit_program(EXIT_FAILURE, "Error while setting the sigint handler");
    }

    init_res(); // initialize every resource

    int client;

    printf("%zu: golibroda spi: \n", getTime());
    while(1) {
        sem_wait(semTab[BARBER]); // sleep
        sem_wait(semTab[FIFO]); // waiting for fifo, to take client
        client = fifo_get_client_on_chair(shmTab);
        sem_post(semTab[FIFO]);
        make_cut(client);

        while(1){
            sem_wait(semTab[FIFO]);
            client = fifo_pop(shmTab);
            if(client!=-1){
                make_cut(client);
                sem_post(semTab[FIFO]);
            }
            else{
                printf("golibroda spi: %zu\n", getTime());
                sem_wait(semTab[BARBER]);
                sem_post(semTab[FIFO]);
                break;
            }
        }
    }

    free_res();

    return 0;
}

