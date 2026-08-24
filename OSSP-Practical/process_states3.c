#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main() {
    pid_t pid;

    printf("Before fork:\n");
    printf("Current PID  = %d\n", getpid());
    printf("Current PPID = %d\n\n", getppid());

    pid = fork();

    if (pid < 0) {
        perror("fork failed");
        exit(1);
    }

    if (pid == 0) {
        /* Child process */

        printf("========== CHILD PROCESS ==========\n");
        printf("Child PID  = %d\n", getpid());
        printf("Child PPID = %d\n", getppid());

        printf("\nChild is running for 10 seconds...\n");

        for (volatile long i = 0; i < 2000000000L; i++);

        printf("Child entering sleep/wait state for 20 seconds...\n");
        sleep(20);

        printf("Child terminating now.\n");
        exit(0);

    } else {
        /* Parent process */

        printf("========== PARENT PROCESS ==========\n");
        printf("Parent PID = %d\n", getpid());
        printf("Child PID  = %d\n", pid);
        printf("Parent PPID = %d\n", getppid());

        printf("\nParent sleeping for 5 seconds...\n");
        sleep(5);

        printf("Parent is waiting for child process...\n");

        wait(NULL);

        printf("Child has terminated.\n");
        printf("Parent terminating now.\n");
    }

    return 0;
}
