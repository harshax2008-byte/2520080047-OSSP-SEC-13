#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

#define SIZE 1000000

int main(void)
{
    int *data = malloc(SIZE * sizeof(int));

    if (data == NULL) {
        perror("malloc");
        return 1;
    }

    for (int i = 0; i < SIZE; i++)
        data[i] = i;

    printf("Parent PID: %d\n", getpid());
    printf("Memory allocated: %zu MB\n",
           (SIZE * sizeof(int)) / (1024 * 1024));

    pid_t pid = fork();

    if (pid < 0) {
        perror("fork");
        free(data);
        return 1;
    }

    if (pid == 0) {
        printf("\nChild process PID: %d\n", getpid());
        printf("Child initially shares memory pages with parent (Copy-on-Write).\n");

        sleep(5);

        // Modify the memory in the child
        for (int i = 0; i < SIZE; i++)
            data[i] = data[i] * 2;

        printf("Child modified the allocated memory.\n");
        printf("data[0] = %d\n", data[0]);

        sleep(5);

        free(data);
        exit(0);
    }
    else {
        printf("\nParent process PID: %d\n", getpid());
        printf("Child PID: %d\n", pid);

        sleep(10);

        printf("Parent still has data[0] = %d\n", data[0]);

        wait(NULL);

        free(data);
    }

    return 0;
}