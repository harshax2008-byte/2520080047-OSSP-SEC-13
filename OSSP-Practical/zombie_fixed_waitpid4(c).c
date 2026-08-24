#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main() {
    pid_t pid;
    int status;

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
        printf("Waiting specifically for child PID = %d\n", pid);

        waitpid(pid, &status, 0);

        printf("Child process collected successfully.\n");

        if (WIFEXITED(status)) {
            printf("Child exited normally with status %d\n",
                   WEXITSTATUS(status));
        }

        printf("Zombie process eliminated.\n");
    }

    return 0;
}
