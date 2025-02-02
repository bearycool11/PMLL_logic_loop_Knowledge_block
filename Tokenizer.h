#ifndef UTF8_TOKENIZER_H
#define UTF8_TOKENIZER_H

#include <stdbool.h> // For boolean type

// Maximum input size for the tokenizer
#define MAX_INPUT_SIZE 100

// Function Prototypes

/**
 * @brief Checks if a byte is the start of a UTF-8 character.
 * 
 * @param byte The byte to check.
 * @return true if the byte is the start of a UTF-8 character, false otherwise.
 */
bool is_utf8_start_byte(unsigned char byte);

/**
 * @brief Checks if a character is a valid delimiter in C/C++ code.
 * 
 * @param c The character to check.
 * @return true if the character is a delimiter, false otherwise.
 */
bool is_c_delimiter(char c);

/**
 * @brief Tokenizes a UTF-8 encoded C/C++ string.
 * 
 * @param input The input string to tokenize.
 */
void c_file_tokenize(const char *input);

#endif // UTF8_TOKENIZER_H
