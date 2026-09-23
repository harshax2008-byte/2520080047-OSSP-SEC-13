#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int global_var = 10;
static int static_var = 20;

void code_function(void)
{
    printf("Inside code_function()\n");
}

int main(void)
{
    int stack_var = 30;
    static int static_local = 40;

    int *heap_var = malloc(sizeof(int));
    *heap_var = 50;

    printf("Process ID (PID)        : %d\n", getpid());

    printf("Code address            : %p\n", (void *)code_function);
    printf("Global variable address : %p\n", (void *)&global_var);
    printf("Static variable address : %p\n", (void *)&static_var);
    printf("Static local address    : %p\n", (void *)&static_local);
    printf("Heap variable address   : %p\n", (void *)heap_var);
    printf("Stack variable address  : %p\n", (void *)&stack_var);

    printf("\nPress Enter to terminate...\n");
    getchar();

    free(heap_var);

    return 0;
}