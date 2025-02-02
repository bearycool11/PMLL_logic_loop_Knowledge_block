#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "io_socket.h"
#include "pml_logic_loop.h"
#include "memory_silo.h"
#include "knowledge.h"
#include "PMLL_ARLL_EFLL.h"

// Global structures for managing sessions
io_socket_t io_socket;
PMLL_ARLL_EFLL_State session_state;
Graph *session_graph = NULL; // Knowledge graph for current session

// Function to send knowledge graph data (e.g., metrics)
void send_graph_node(io_socket_t *io_socket, const char *node_name, pml_metrics_t *metrics) {
    if (!io_socket || !node_name || !metrics) {
        fprintf(stderr, "Error: Invalid parameters for sending graph node\n");
        return;
    }

    size_t node_name_length = strlen(node_name) + 1; // Include null terminator
    size_t metrics_size = sizeof(pml_metrics_t);
    size_t total_size = node_name_length + metrics_size;

    // Allocate memory for the data to be sent
    char *data = malloc(total_size);
    if (!data) {
        perror("Memory allocation failed");
        return;
    }

    // Copy the node name and metrics into the data buffer
    memcpy(data, node_name, node_name_length);
    memcpy(data + node_name_length, metrics, metrics_size);

    // Send the data through the socket
    if (io_socket_send(io_socket->socket, data, total_size) < 0) {
        fprintf(stderr, "Failed to send data\n");
    } else {
        printf("Knowledge graph data sent successfully\n");
    }

    free(data);
}

// Function to initialize the PML logic loop
void pml_logic_loop_init_wrapper(io_socket_t *io_socket, int memory_silo_id) {
    if (!io_socket) {
        fprintf(stderr, "Error: IO socket instance is null\n");
        return;
    }
    pml_logic_loop_init(memory_silo_id, io_socket->socket);
}

// Function to handle the main PML logic loop with socket communication
void pml_logic_loop_process_wrapper(io_socket_t *io_socket, memory_silo_t *silo) {
    if (!io_socket || !silo) {
        fprintf(stderr, "Error: Invalid parameters for PML logic loop processing\n");
        return;
    }

    // Assuming the silo contains knowledge graph or state data
    send_graph_node(io_socket, "Model A", &silo->metrics);

    // Process the PML logic loop
    pml_logic_loop_process(io_socket->socket, NULL, 0);
}

// Function to initialize the IO socket
int io_socket_init_wrapper(io_socket_t *io_socket, const char *ip, int port) {
    return io_socket_init(io_socket, ip, port);
}

// Function to clean up and close the IO socket
void io_socket_cleanup(io_socket_t *io_socket) {
    if (io_socket) {
        io_socket_close(io_socket);
        printf("IO socket cleaned up successfully\n");
    }
}

// Function to save the state of the memory silo
void save_memory_silo(const char* filename, memory_silo_t* memory_silo) {
    if (!memory_silo) {
        fprintf(stderr, "Error: Memory silo is null, cannot save\n");
        return;
    }
    FILE* file = fopen(filename, "wb");
    if (file == NULL) {
        perror("Failed to open file for saving Memory Silo");
        return;
    }
    fwrite(memory_silo, sizeof(memory_silo_t), 1, file);
    fclose(file);
    printf("Memory silo state saved to %s\n", filename);
}

// Function to load the state of the memory silo
memory_silo_t* load_memory_silo(const char* filename) {
    FILE* file = fopen(filename, "rb");
    if (file == NULL) {
        perror("Failed to open file for loading Memory Silo");
        return NULL;
    }

    memory_silo_t* memory_silo = malloc(sizeof(memory_silo_t));
    if (memory_silo == NULL) {
        perror("Memory allocation failed for Memory Silo");
        fclose(file);
        return NULL;
    }
    fread(memory_silo, sizeof(memory_silo_t), 1, file);
    fclose(file);

    printf("Memory silo state loaded from %s\n", filename);
    return memory_silo;
}

// Function to initialize session
void init_session(const char *ip, int port, int max_retries, int feedback_threshold) {
    if (io_socket_init(&io_socket, ip, port) < 0) {
        fprintf(stderr, "Failed to initialize IO socket\n");
        exit(EXIT_FAILURE);
    }

    pmll_arll_efll_init(&session_state, max_retries, feedback_threshold);
    session_graph = create_graph(1024); // Assuming create_graph from knowledge.h
    if (!session_graph) {
        fprintf(stderr, "Failed to initialize session graph\n");
        exit(EXIT_FAILURE);
    }
}

// Function to process input from chatlog and update knowledge graph
void process_chatlog(const char *input) {
    // Tokenize and add to session graph
    char *token = strtok(strdup(input), " \t\n");
    while (token) {
        Node *node = create_node(token, 0);
        add_node(session_graph, node);
        token = strtok(NULL, " \t\n");
    }

    // Process via PML logic loop with new data
    pml_logic_loop_process_wrapper(&io_socket, NULL); // Assuming NULL silo as we're using session_graph
}

// Function to reward or penalize based on ARLL logic
void reward_session(const char *topic, bool is_good) {
    ARLLRewards rewards = {0, 0};
    arll_reward_memory(topic, &rewards, is_good);
    printf("Session %s rewarded: Good/True: %d, False/Good: %d\n", topic, rewards.good_true_rewards, rewards.false_good_rewards);
}

// Function to judge the validity of the session's knowledge using EFLL
bool validate_session() {
    return efll_judge_memory(session_graph);
}

// Function to save session state
void save_session_state(const char* filename) {
    FILE* file = fopen(filename, "wb");
    if (file == NULL) {
        perror("Failed to open file for saving session");
        return;
    }
    fwrite(&session_state, sizeof(PMLL_ARLL_EFLL_State), 1, file);
    fclose(file);
    printf("Session state saved to %s\n", filename);
}

// Function to load session state
void load_session_state(const char* filename) {
    FILE* file = fopen(filename, "rb");
    if (file == NULL) {
        perror("Failed to open file for loading session");
        return;
    }
    fread(&session_state, sizeof(PMLL_ARLL_EFLL_State), 1, file);
    fclose(file);
    printf("Session state loaded from %s\n", filename);
}

// Main function to demonstrate persistence with socket, PML logic loop, and session management
int main() {
    init_session("127.0.0.1", 8080, 5, 3); // Initialize with example parameters

    memory_silo_t silo = {0}; // Example memory silo for additional processing
    pml_logic_loop_init_wrapper(&io_socket, silo.id);

    // Load previous state from files if needed
    memory_silo_t* loaded_memory_silo = load_memory_silo("memory_silo_state.dat");
    if (loaded_memory_silo) {
        printf("Loaded memory silo ID: %d\n", loaded_memory_silo->id);
        free(loaded_memory_silo); // Free after use
    }

    // Example chatlog session
    const char *chat_inputs[] = {
        "Hello, how are you?",
        "I'm doing well, thanks for asking!",
        "What's the weather like today?",
        "It's sunny and warm."
    };
    size_t num_inputs = sizeof(chat_inputs) / sizeof(chat_inputs[0]);

    for (size_t i = 
