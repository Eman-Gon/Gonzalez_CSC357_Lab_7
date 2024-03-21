#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>

void alarm_handler(int signum) {
    printf("child process.\n");
    exit(1);
}
int main(int argc, char *argv[]) {
    if (argc < 3) {
        printf("invalid\n", argv[0]);
        return 1;
    }

    int timeout = atoi(argv[1]);
    if (timeout <= 0) {
        printf("Invalid timeout value\n");
        return 1;
    }

    pid_t pid = fork();
    if (pid == -1) {
        printf("Failed\n");
        return 1;
    } else if (pid == 0) {
        execvp(argv[2], &argv[2]);
        printf("Failed\n");
        return 1;
    } else {
        struct sigaction sa;
        sa.sa_handler = alarm_handler;
        sigemptyset(&sa.sa_mask);
        sa.sa_flags = 0;

        if (sigaction(SIGALRM, &sa, NULL) == -1) {
            exit(1);
        }
        alarm(timeout);

        int status;
        if (waitpid(pid, &status, 0) == -1) {
            printf("Failed
            \n");
            return 1;
        }
        if (WIFEXITED(status)) {

            return WEXITSTATUS(status);
        } else {
            printf("Child process\n");
            return 1;
        }
    }
    return 0;
}