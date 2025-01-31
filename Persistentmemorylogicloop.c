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
#include "PMLL.h"

// Initialize state, loading from file if available; otherwise, set defaults
void initialize_state(PMLLState *state) {
    if (!load_state(state)) {
        state->iteration_count = 0;
        state->last_value = 1.0; // Default starting value
        strcpy(state->last_input, ""); // Initialize empty input memory
        log_event("Initialized new state.");
    } else {
        char log_msg[256];
        snprintf(log_msg, sizeof(log_msg),
                 "Resumed state: Iteration %d, Last Value: %f, Last Input: %s",
                 state->iteration_count, state->last_value, state->last_input);
        log_event(log_msg);
    }
}

// Save state to a persistent file
void save_state(const PMLLState *state) {
    FILE *file = fopen(STATE_FILE, "wb");
    if (!file) {
        perror("Error saving state");
        log_event("ERROR: Unable to save state.");
        return;
    }
    fwrite(state, sizeof(PMLLState), 1, file);
    fclose(file);
    log_event("State saved successfully.");
}

// Load state from the persistent file
bool load_state(PMLLState *state) {
    FILE *file = fopen(STATE_FILE, "rb");
    if (!file) {
        log_event("No previous state found, starting fresh.");
        return false;
    }
    fread(state, sizeof(PMLLState), 1, file);
    fclose(file);
    return true;
}

// Log system events to a file
void log_event(const char *message) {
    FILE *log_file = fopen(LOG_FILE, "a");
    if (log_file) {
        fprintf(log_file, "%s\n", message);
        fclose(log_file);
    }
}

// Detect if a novel topic is introduced based on changes in input context
bool NovelTopic(const char *input, PMLLState *state) {
    return strcmp(input, state->last_input) != 0;
}

// Process a novel input, updating the persistent state
void NovelInput(const char *input, PMLLState *state) {
    if (NovelTopic(input, state)) {
        strncpy(state->last_input, input, sizeof(state->last_input) - 1);
        state->last_input[sizeof(state->last_input) - 1] = '\0'; // Null-terminate
        log_event("Detected Novel Input");
        save_state(state);
    }
}

// The persistent memory logic loop (ensures state is retained indefinitely)
void logic_loop(PMLLState *state) {
    char input[256];

    while (1) {  // Infinite loop ensures persistence
        printf("Enter new input (or type 'exit' to quit): ");
        fgets(input, sizeof(input), stdin);
        input[strcspn(input, "\n")] = '\0'; // Remove newline

        if (strcmp(input, "exit") == 0) {
            printf("Exiting loop...\n");
            log_event("User exited loop.");
            save_state(state); // Save final state before exiting
            break;
        }

        NovelInput(input, state); // Process novel input

        state->iteration_count++;
        state->last_value *= 1.1; // Example computation

        char log_msg[256];
        snprintf(log_msg, sizeof(log_msg),
                 "Iteration: %d, Last Value: %f, Last Input: %s",
                 state->iteration_count, state->last_value, state->last_input);
        log_event(log_msg);

        printf("%s\n", log_msg); // Console output
        save_state(state); // Persist state every iteration

        sleep(1); // Simulated processing delay
    }
}
