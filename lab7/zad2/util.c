#include "communication.h"


void exit_program(int status, char *message) {
    status == EXIT_SUCCESS? printf("%s\n",message) : perror(message);
    exit(status);
}

__time_t getTime() {
    struct timespec time;
    if (clock_gettime(CLOCK_MONOTONIC, &time) == -1)
        exit_program(EXIT_FAILURE,"Get time problem");
    return time.tv_nsec/1000;
}

