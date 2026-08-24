#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main() {
    int i;
    pid_t pid;

    printf("Parent process started. PID = %d\n", getpid());

    for (i = 0; i < 3; i++) {
        pid = fork();

        if (pid < 0) {
            perror("fork failed");
            exit(1);
        }

        if (pid == 0) {
            printf("Child %d started. PID = %d, Parent PID = %d\n",
                   i + 1, getpid(), getppid());

            sleep(2 + i);

            printf("Child %d finished. PID = %d\n",
                   i + 1, getpid());

            exit(0);
        }
    }

    /* Parent waits for any child process */
    for (i = 0; i < 3; i++) {
        pid = wait(NULL);

        printf("Parent collected child with PID = %d\n", pid);
    }

    printf("All child processes completed.\n");

    return 0;
}