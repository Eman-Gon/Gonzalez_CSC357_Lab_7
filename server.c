#define _GNU_SOURCE
#include "net.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <sys/wait.h>

#define PORT 32454

void handle_request(int nfd) {
    FILE *network = fdopen(nfd, "r");
    char *line = NULL;
    size_t size;
    ssize_t num;

    if (!network) {
        perror("fdopen");
        close(nfd);
        return;
    }

    while ((num = getline(&line, &size, network)) >= 0)
        write(nfd, line, num);

    free(line);
    fclose(network);
}

void sigchld_handler(int signum) {
    int status;
    pid_t pid;

    while ((pid = waitpid(-1, &status, WNOHANG)) > 0)
        printf("Child process %d terminated\n", pid);
}

void run_service(int fd) {
    signal(SIGCHLD, sigchld_handler);

    while (1) {
        int nfd = accept_connection(fd);
        if (nfd != -1) {
            printf("Connected\n");

            pid_t pid = fork();
            if (pid == -1) {
                perror("fork");
                exit(1);
            } else if (pid == 0) {
                close(fd);
                handle_request(nfd);
                close(nfd);
                exit(0);
            } else {
                close(nfd);
            }
        }
    }
}

int main(void) {
    int fd = create_service(PORT);
    if (fd == -1) {
        perror(0);
        exit(1);
    }

    printf("Listening: %d\n", PORT);
    run_service(fd);

    close(fd);
    return 0;
}