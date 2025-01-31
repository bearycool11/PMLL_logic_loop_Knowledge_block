#ifndef PMLL_H
#define PMLL_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>  // For sleep function (POSIX). Use <windows.h> for Windows.
#include <pthread.h> // Optional: For multi-threading

#define STATE_FILE "pmll_state.dat"  // Persistent storage file
#define LOG_FILE   "pmll_log.txt"    // Log file for debugging

// Define a structure to represent the persistent loop state
typedef struct {
    int iteration_count;  // Number of iterations executed
    double last_value;    // Example persistent variable
    char last_input[256]; // Stores last processed input
} PMLLState;

// Function prototypes
void initialize_state(PMLLState *state);
void save_state(const PMLLState *state);
bool load_state(PMLLState *state);
void logic_loop(PMLLState *state);
void log_event(const char *message);
bool NovelTopic(const char *input, PMLLState *state);
void NovelInput(const char *input, PMLLState *state);

#endif // PMLL_H
