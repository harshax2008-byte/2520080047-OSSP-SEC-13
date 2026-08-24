#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>

int main() {
    pid_t pid;

    pid = fork();

    if (pid < 0) {
        perror("fork failed");
        exit(1);
    }

    if (pid == 0) {
        printf("Child process started.\n");
        printf("Child PID = %d\n", getpid());

        sleep(2);

        printf("Child process terminating.\n");

        exit(0);
    }
    else {
        printf("Parent PID = %d\n", getpid());
        printf("Child PID = %d\n", pid);

        printf("Parent will sleep for 30 seconds.\n");
        printf("During this time, the terminated child can become a zombie.\n");

        sleep(30);

        printf("Parent process ending.\n");

        return 0;
    }
}