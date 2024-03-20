#define _GNU_SOURCE
#include "net.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <sys/wait.h>
#include <errno.h>

#define PORT 32454

void handle_request(int nfd)
{
    FILE *network = fdopen(nfd, "r");
    char *line = NULL;
    size_t size;
    ssize_t num;
    
    if (network == NULL)
    {
        perror("fdopen");
        close(nfd);
        return;
    }
    
    while ((num = getline(&line, &size, network)) >= 0)
    {
        write(nfd, line, num);
    }
    
    free(line);
    fclose(network);
}

void sigchld_handler(int sig)
{
    int saved_errno = errno;
    while (waitpid(-1, NULL, WNOHANG) > 0);
    errno = saved_errno;
}

void run_service(int fd)
{
    struct sigaction sa;
    sa.sa_handler = sigchld_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    if (sigaction(SIGCHLD, &sa, NULL) == -1)
    {
        perror("sigaction");
        exit(1);
    }
    
    while (1)
    {
        int nfd = accept_connection(fd);
        if (nfd != -1)
        {
            pid_t pid = fork();
            if (pid == -1)
            {
                perror("fork");
                close(nfd);
            }
            else if (pid == 0)
            {
                // Child process
                close(fd);
                printf("Connection established\n");
                handle_request(nfd);
                printf("Connection closed\n");
                exit(0);
            }
            else
            {
                // Parent process
                close(nfd);
            }
        }
    }
}

int main(void)
{
    int fd = create_service(PORT);
    if (fd == -1)
    {
        perror(0);
        exit(1);
    }
    
    printf("Listening on port: %d\n", PORT);
    run_service(fd);
    close(fd);
    
    return 0;
}