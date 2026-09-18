#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>
#include <errno.h>

#define INITIAL_BUFFER_SIZE 16

/*
 * A node in the command-history linked list.
 */
typedef struct HistoryNode {
    char *command;
    struct HistoryNode *next;
} HistoryNode;


/*
 * Enable raw terminal mode.
 *
 * In raw mode:
 * - Characters are received one at a time.
 * - Enter is not automatically processed as a complete line.
 * - Arrow keys can be detected through escape sequences.
 * - Echo is disabled so that we can control what appears on screen.
 */
static int enable_raw_mode(struct termios *original)
{
    struct termios raw;

    if (tcgetattr(STDIN_FILENO, original) == -1) {
        perror("tcgetattr");
        return -1;
    }

    raw = *original;

    raw.c_lflag &= ~(ICANON | ECHO);
    raw.c_iflag &= ~(IXON | ICRNL);

    raw.c_cc[VMIN] = 1;
    raw.c_cc[VTIME] = 0;

    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1) {
        perror("tcsetattr");
        return -1;
    }

    return 0;
}


/*
 * Restore normal terminal mode.
 */
static void disable_raw_mode(const struct termios *original)
{
    tcsetattr(STDIN_FILENO, TCSAFLUSH, original);
}


/*
 * Clear the current terminal line.
 *
 * \r     -> move cursor to beginning
 * \033[K -> ANSI escape sequence that clears from cursor to end
 */
static void clear_line(void)
{
    printf("\r\033[K");
}


/*
 * Display the current input buffer.
 */
static void display_buffer(const char *buffer)
{
    clear_line();
    printf("> %s", buffer);
    fflush(stdout);
}


/*
 * Dynamically resize the input buffer.
 *
 * The capacity is doubled until it is large enough.
 */
static int resize_buffer(
    char **buffer,
    size_t *capacity,
    size_t required
)
{
    size_t new_capacity;
    char *new_buffer;

    if (required <= *capacity) {
        return 0;
    }

    new_capacity = *capacity;

    while (new_capacity < required) {
        if (new_capacity > (size_t)-1 / 2) {
            return -1;
        }

        new_capacity *= 2;
    }

    new_buffer = realloc(*buffer, new_capacity);

    if (new_buffer == NULL) {
        return -1;
    }

    *buffer = new_buffer;
    *capacity = new_capacity;

    return 0;
}


/*
 * Add a command to the history linked list.
 */
static int add_history(
    HistoryNode **head,
    const char *command
)
{
    HistoryNode *new_node;
    HistoryNode *current;

    new_node = malloc(sizeof(HistoryNode));

    if (new_node == NULL) {
        return -1;
    }

    new_node->command = malloc(strlen(command) + 1);

    if (new_node->command == NULL) {
        free(new_node);
        return -1;
    }

    strcpy(new_node->command, command);
    new_node->next = NULL;

    if (*head == NULL) {
        *head = new_node;
        return 0;
    }

    current = *head;

    while (current->next != NULL) {
        current = current->next;
    }

    current->next = new_node;

    return 0;
}


/*
 * Count the number of commands in history.
 */
static size_t history_count(const HistoryNode *head)
{
    size_t count = 0;

    while (head != NULL) {
        count++;
        head = head->next;
    }

    return count;
}


/*
 * Return the history command at a particular zero-based index.
 */
static const char *get_history(
    const HistoryNode *head,
    size_t index
)
{
    size_t current_index = 0;

    while (head != NULL) {

        if (current_index == index) {
            return head->command;
        }

        current_index++;
        head = head->next;
    }

    return NULL;
}


/*
 * Free the complete history linked list.
 */
static void free_history(HistoryNode *head)
{
    HistoryNode *next;

    while (head != NULL) {

        next = head->next;

        free(head->command);
        free(head);

        head = next;
    }
}


/*
 * Copy a history command into the input buffer.
 */
static int load_history(
    char **buffer,
    size_t *capacity,
    size_t *length,
    const char *command
)
{
    size_t required;

    required = strlen(command) + 1;

    if (resize_buffer(buffer, capacity, required) == -1) {
        return -1;
    }

    strcpy(*buffer, command);

    *length = strlen(command);

    display_buffer(*buffer);

    return 0;
}


/*
 * Execute a command.
 *
 * For this lab demonstration, commands are displayed rather than
 * passed to the operating system.
 */
static void execute_command(const char *command)
{
    if (strcmp(command, "help") == 0) {

        printf("Available commands:\n");
        printf("  help  - Display this help message\n");
        printf("  history - Display command history\n");
        printf("  exit  - Exit the program\n");
        printf("  quit  - Exit the program\n");

    } else {

        printf("Command entered: %s\n", command);
    }
}


/*
 * Display all commands in history.
 */
static void print_history(const HistoryNode *head)
{
    size_t number = 1;

    if (head == NULL) {
        printf("No command history.\n");
        return;
    }

    while (head != NULL) {

        printf("%zu  %s\n", number, head->command);

        number++;
        head = head->next;
    }
}


/*
 * Main program.
 */
int main(void)
{
    struct termios original_terminal;

    HistoryNode *history = NULL;

    char *buffer = NULL;

    size_t capacity = INITIAL_BUFFER_SIZE;
    size_t length = 0;

    size_t history_position;

    int running = 1;

    /*
     * Allocate the initial input buffer.
     */
    buffer = malloc(capacity);

    if (buffer == NULL) {
        perror("malloc");
        return EXIT_FAILURE;
    }

    buffer[0] = '\0';

    /*
     * Enable raw keyboard input.
     */
    if (enable_raw_mode(&original_terminal) == -1) {
        free(buffer);
        return EXIT_FAILURE;
    }

    printf("> ");
    fflush(stdout);

    history_position = history_count(history);

    while (running) {

        char c;

        ssize_t bytes_read;

        bytes_read = read(STDIN_FILENO, &c, 1);

        if (bytes_read == -1) {

            if (errno == EINTR) {
                continue;
            }

            perror("read");
            break;
        }

        if (bytes_read == 0) {
            break;
        }


        /*
         * ENTER KEY
         */
        if (c == '\n' || c == '\r') {

            buffer[length] = '\0';

            printf("\n");

            if (length > 0) {

                /*
                 * Exit conditions.
                 */
                if (strcmp(buffer, "exit") == 0 ||
                    strcmp(buffer, "quit") == 0) {

                    running = 0;

                } else {

                    /*
                     * Add command to history.
                     */
                    if (add_history(&history, buffer) == -1) {

                        fprintf(
                            stderr,
                            "Error: unable to store command history.\n"
                        );

                        running = 0;

                    } else {

                        /*
                         * Built-in history command.
                         */
                        if (strcmp(buffer, "history") == 0) {

                            print_history(history);

                        } else {

                            execute_command(buffer);
                        }
                    }
                }
            }

            /*
             * Reset input buffer.
             */
            length = 0;
            buffer[0] = '\0';

            history_position = history_count(history);

            if (running) {
                printf("> ");
                fflush(stdout);
            }
        }


        /*
         * BACKSPACE KEY
         *
         * ASCII 127 is the usual Backspace/Delete character
         * sent by Ubuntu terminals.
         */
        else if (c == 127 || c == '\b') {

            if (length > 0) {

                length--;

                buffer[length] = '\0';

                printf("\b \b");
                fflush(stdout);
            }
        }


        /*
         * ESCAPE SEQUENCE
         *
         * Arrow keys generate sequences such as:
         *
         * UP    = ESC [ A
         * DOWN  = ESC [ B
         * RIGHT = ESC [ C
         * LEFT  = ESC [ D
         */
        else if (c == '\033') {

            char sequence[2];

            if (read(STDIN_FILENO, &sequence[0], 1) != 1) {
                continue;
            }

            if (sequence[0] != '[') {
                continue;
            }

            if (read(STDIN_FILENO, &sequence[1], 1) != 1) {
                continue;
            }


            /*
             * UP ARROW
             *
             * Navigate to previous command.
             */
            if (sequence[1] == 'A') {

                size_t count = history_count(history);

                if (count > 0 && history_position > 0) {

                    history_position--;

                    {
                        const char *command =
                            get_history(
                                history,
                                history_position
                            );

                        if (command != NULL) {

                            if (load_history(
                                    &buffer,
                                    &capacity,
                                    &length,
                                    command
                                ) == -1) {

                                fprintf(
                                    stderr,
                                    "\nError: unable to load history.\n"
                                );

                                running = 0;
                            }
                        }
                    }
                }
            }


            /*
             * DOWN ARROW
             *
             * Navigate to next command.
             */
            else if (sequence[1] == 'B') {

                size_t count = history_count(history);

                if (history_position + 1 < count) {

                    history_position++;

                    {
                        const char *command =
                            get_history(
                                history,
                                history_position
                            );

                        if (command != NULL) {

                            if (load_history(
                                    &buffer,
                                    &capacity,
                                    &length,
                                    command
                                ) == -1) {

                                fprintf(
                                    stderr,
                                    "\nError: unable to load history.\n"
                                );

                                running = 0;
                            }
                        }
                    }

                } else {

                    /*
                     * Moving below the newest history item
                     * produces an empty input buffer.
                     */
                    history_position = count;

                    length = 0;
                    buffer[0] = '\0';

                    display_buffer(buffer);
                }
            }
        }


        /*
         * NORMAL PRINTABLE CHARACTER
         */
        else if (c >= 32 && c <= 126) {

            /*
             * Ensure that there is always room for:
             *
             * the new character + '\0'
             */
            if (resize_buffer(
                    &buffer,
                    &capacity,
                    length + 2
                ) == -1) {

                fprintf(
                    stderr,
                    "\nError: unable to resize input buffer.\n"
                );

                running = 0;
                continue;
            }

            buffer[length] = c;

            length++;

            buffer[length] = '\0';

            putchar(c);
            fflush(stdout);
        }
    }


    /*
     * Always restore the terminal before exiting.
     */
    disable_raw_mode(&original_terminal);

    printf("\n");

    /*
     * Release dynamically allocated input buffer.
     */
    free(buffer);

    /*
     * Release the complete history linked list.
     */
    free_history(history);

    return EXIT_SUCCESS;
}