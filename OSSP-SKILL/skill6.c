#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_TOKENS 100
#define MAX_TOKEN_LEN 100

typedef enum {
    TOKEN_WORD,
    TOKEN_PIPE,
    TOKEN_INPUT,
    TOKEN_OUTPUT,
    TOKEN_APPEND
} TokenType;

typedef struct {
    TokenType type;
    char value[MAX_TOKEN_LEN];
} Token;

Token tokens[MAX_TOKENS];
int token_count = 0;

void add_token(TokenType type, const char *value) {
    if (token_count >= MAX_TOKENS) {
        printf("Error: Too many tokens\n");
        return;
    }

    tokens[token_count].type = type;
    strncpy(tokens[token_count].value, value, MAX_TOKEN_LEN - 1);
    tokens[token_count].value[MAX_TOKEN_LEN - 1] = '\0';

    token_count++;
}

void tokenize(char *input) {
    int i = 0;

    while (input[i] != '\0') {

        // Ignore whitespace
        if (isspace((unsigned char)input[i])) {
            i++;
            continue;
        }

        // Pipe
        if (input[i] == '|') {
            add_token(TOKEN_PIPE, "|");
            i++;
        }

        // Input redirection
        else if (input[i] == '<') {
            add_token(TOKEN_INPUT, "<");
            i++;
        }

        // Output / append
        else if (input[i] == '>') {

            if (input[i + 1] == '>') {
                add_token(TOKEN_APPEND, ">>");
                i += 2;
            } else {
                add_token(TOKEN_OUTPUT, ">");
                i++;
            }
        }

        // Word
        else {
            char word[MAX_TOKEN_LEN];
            int j = 0;

            while (input[i] != '\0' &&
                   !isspace((unsigned char)input[i]) &&
                   input[i] != '|' &&
                   input[i] != '<' &&
                   input[i] != '>') {

                if (j < MAX_TOKEN_LEN - 1) {
                    word[j++] = input[i];
                }

                i++;
            }

            word[j] = '\0';

            if (j > 0) {
                add_token(TOKEN_WORD, word);
            }
        }
    }
}

const char *token_name(TokenType type) {
    switch (type) {
        case TOKEN_WORD:
            return "WORD";
        case TOKEN_PIPE:
            return "PIPE";
        case TOKEN_INPUT:
            return "INPUT";
        case TOKEN_OUTPUT:
            return "OUTPUT";
        case TOKEN_APPEND:
            return "APPEND";
        default:
            return "UNKNOWN";
    }
}

void print_tokens() {
    printf("\n--- Token Stream ---\n");

    for (int i = 0; i < token_count; i++) {
        printf("%d: %-8s : %s\n",
               i,
               token_name(tokens[i].type),
               tokens[i].value);
    }
}

int validate_tokens() {

    if (token_count == 0) {
        printf("Empty command.\n");
        return 0;
    }

    // Command cannot start with pipe
    if (tokens[0].type == TOKEN_PIPE) {
        printf("Syntax Error: Command cannot start with '|'\n");
        return 0;
    }

    // Command cannot end with pipe
    if (tokens[token_count - 1].type == TOKEN_PIPE) {
        printf("Syntax Error: Command cannot end with '|'\n");
        return 0;
    }

    // Check consecutive pipes
    for (int i = 0; i < token_count - 1; i++) {

        if (tokens[i].type == TOKEN_PIPE &&
            tokens[i + 1].type == TOKEN_PIPE) {

            printf("Syntax Error: Consecutive pipes\n");
            return 0;
        }
    }

    printf("Token stream is valid.\n");
    return 1;
}

int main() {

    char input[500];

    printf("Enter command: ");

    if (fgets(input, sizeof(input), stdin) == NULL) {
        return 1;
    }

    // Remove newline
    input[strcspn(input, "\n")] = '\0';

    tokenize(input);

    print_tokens();

    validate_tokens();

    return 0;
}