// producer_consumer.c

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>

#define NUM_ITEMS 100000

int main() {
    int pipefd[2];
    pid_t pid;

    if (pipe(pipefd) == -1) {
        perror("pipe");
        return 1;
    }

    pid = fork();

    if (pid < 0) {
        perror("fork");
        return 1;
    }

    if (pid == 0) {
        // Child = Consumer
        close(pipefd[1]);

        int value;
        long count = 0;

        while (read(pipefd[0], &value, sizeof(value)) > 0) {
            count++;
        }

        close(pipefd[0]);

        printf("Consumer received %ld items\n", count);
        exit(0);

    } else {
        // Parent = Producer
        close(pipefd[0]);

        struct timespec start, end;

        clock_gettime(CLOCK_MONOTONIC, &start);

        for (int i = 1; i <= NUM_ITEMS; i++) {
            if (write(pipefd[1], &i, sizeof(i)) == -1) {
                perror("write");
                break;
            }
        }

        close(pipefd[1]);

        wait(NULL);

        clock_gettime(CLOCK_MONOTONIC, &end);

        double elapsed =
            (end.tv_sec - start.tv_sec) +
            (end.tv_nsec - start.tv_nsec) / 1e9;

        printf("Communication time: %.6f seconds\n", elapsed);
        printf("Data items: %d\n", NUM_ITEMS);
        printf("Throughput: %.2f items/second\n",
               NUM_ITEMS / elapsed);
    }

    return 0;
}