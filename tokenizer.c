#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>

#define MAX_INPUT_SIZE 100

// Function to check if a byte is the start of a UTF-8 character
bool is_utf8_start_byte(unsigned char byte) {
    return (byte & 0xC0) != 0x80; // Start byte does not have the pattern 10xxxxxx
}

// Function to check if a character is a valid C token delimiter
bool is_c_delimiter(char c) {
    const char *delimiters = " \t\n(){}[];,=*+-/<>!&|^~";
    return strchr(delimiters, c) != NULL;
}

// Function to tokenize a .c or .h file string
void c_file_tokenize(const char *input) {
    const char *start = input;
    const char *current = input;

    printf("Tokenizing input (C/C++ style): \"%s\"\n", input);

    while (*current) {
        // Handle comments (single-line and multi-line)
        if (*current == '/' && *(current + 1) == '/') {
            // Single-line comment
            printf("Token: //");
            current += 2;
            while (*current && *current != '\n') {
                putchar(*current++);
            }
            printf("\n");
            continue;
        } else if (*current == '/' && *(current + 1) == '*') {
            // Multi-line comment
            printf("Token: /*");
            current += 2;
            while (*current && !(*current == '*' && *(current + 1) == '/')) {
                putchar(*current++);
            }
            if (*current == '*' && *(current + 1) == '/') {
                printf("*/\n");
                current += 2;
            }
            continue;
        }

        // Handle string literals
        if (*current == '"') {
            printf("Token: \"");
            current++;
            while (*current && *current != '"') {
                if (*current == '\\' && *(current + 1)) {
                    putchar(*current++);
                }
                putchar(*current++);
            }
            if (*current == '"') {
                printf("\"\n");
                current++;
            }
            continue;
        }

        // Handle character literals
        if (*current == '\'') {
            printf("Token: '");
            current++;
            while (*current && *current != '\'') {
                if (*current == '\\' && *(current + 1)) {
                    putchar(*current++);
                }
                putchar(*current++);
            }
            if (*current == '\'') {
                printf("'\n");
                current++;
            }
            continue;
        }

        // Check if the current character is a delimiter
        if (is_c_delimiter(*current)) {
            // Print the token before the delimiter
            if (current > start) {
                printf("Token: ");
                fwrite(start, 1, current - start, stdout);
                printf("\n");
            }
            // Print the delimiter as its own token
            printf("Token: %c\n", *current);
            current++;
            start = current;
        } else {
            // Handle UTF-8 character traversal or normal characters
            if (is_utf8_start_byte((unsigned char)*current)) {
                current++;
                while (*current && (*current & 0xC0) == 0x80) {
                    current++; // Continue traversing multi-byte UTF-8 character
                }
            } else {
                current++;
            }
        }
    }

    // Print the last token if there is one
    if (current > start) {
        printf("Token: ");
        fwrite(start, 1, current - start, stdout);
        printf("\n");
    }
}

int main() {
    char input[MAX_INPUT_SIZE];

    printf("Enter a C/C++ code snippet to tokenize: ");
    if (fgets(input, MAX_INPUT_SIZE, stdin) == NULL) {
        fprintf(stderr, "Error reading input.\n");
        return EXIT_FAILURE;
    }
    input[strcspn(input, "\n")] = '\0'; // Remove the newline character

    c_file_tokenize(input);

    return 0;
}
