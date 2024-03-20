#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>

int main(int argc, char *argv[]) {
  if (argc < 3) {
    fprintf(stderr, "Wrong format\n");
    exit(1);
  }
    int timeout = atoi(argv[1]);
    if (timeout <= 0) {
        printf("Invalid\n");
        return 1;
    }

    pid_t pid = fork();
    if (pid == -1) {
        printf("Fail\n");
        return 1;
    } else if (pid == 0) {

        execvp(argv[2], &argv[2]);
        printf("Fail\n");
        return 1;
    } else {

        sleep(timeout);
        if (kill(pid, SIGKILL) == -1) {
            printf("Failed \n");
            return 1;
        }
        int status;
        if (waitpid(pid, &status, 0) == -1) {
            printf("Failed \n");
            return 1;
        }
        if (WIFEXITED(status)) {

            return WEXITSTATUS(status);
        } else {

            printf("Cterminated\n");
            return 1;
        }
    }

    return 0;
}