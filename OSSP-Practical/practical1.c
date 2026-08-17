#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>

int main()
{
    char command[100];
    char *args[20];
    char *token;
    int i = 0;

    printf("Enter a Linux command: ");
    fgets(command, sizeof(command), stdin);

    // Remove newline character
    command[strcspn(command, "\n")] = '\0';

    // Split command into arguments
    token = strtok(command, " ");
    while (token != NULL)
    {
        args[i++] = token;
        token = strtok(NULL, " ");
    }
    args[i] = NULL;

    pid_t pid = fork();

    if (pid < 0)
    {
        perror("Fork failed");
        exit(1);
    }

    else if (pid == 0)
    {
        // Child process
        printf("\nChild Process:\n");
        printf("Child PID: %d\n", getpid());
        printf("Executing command...\n");

        execvp(args[0], args);

        // Executes only if exec fails
        perror("Execution failed");
        exit(1);
    }

    else
    {
        // Parent process
        printf("\nParent Process:\n");
        printf("Parent PID: %d\n", getpid());
        printf("Waiting for child process...\n");

        wait(NULL);

        printf("Child process completed.\n");
    }

    return 0;
}