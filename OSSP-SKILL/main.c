#include <stdio.h>
#include <string.h>

int main() {
    char command[100];

    while (1) {
        printf("> ");
        fflush(stdout);

        if (fgets(command, sizeof(command), stdin) == NULL) {
            break;
        }

        command[strcspn(command, "\n")] = '\0';

        if (strcmp(command, "exit") == 0 ||
            strcmp(command, "quit") == 0) {
            printf("Exiting program...\n");
            break;
        }

        printf("You entered: %s\n", command);
    }

    return 0;
