#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main() {
    pid_t child1, child2, child3;

    printf("Parent PID = %d\n", getpid());

    child1 = fork();

    if (child1 == 0) {
        printf("Child 1 started. PID = %d\n", getpid());
        sleep(5);
        printf("Child 1 finished.\n");
        exit(0);
    }

    child2 = fork();

    if (child2 == 0) {
        printf("Child 2 started. PID = %d\n", getpid());
        sleep(2);
        printf("Child 2 finished.\n");
        exit(0);
    }

    child3 = fork();

    if (child3 == 0) {
        printf("Child 3 started. PID = %d\n", getpid());
        sleep(3);
        printf("Child 3 finished.\n");
        exit(0);
    }

    printf("\nParent waiting specifically for Child 1: PID = %d\n",
           child1);

    waitpid(child1, NULL, 0);

    printf("Child 1 has been collected.\n");

    printf("Now waiting for Child 2.\n");
    waitpid(child2, NULL, 0);

    printf("Child 2 has been collected.\n");

    printf("Now waiting for Child 3.\n");
    waitpid(child3, NULL, 0);

    printf("Child 3 has been collected.\n");

    printf("All children synchronized successfully.\n");

    return 0;
}
