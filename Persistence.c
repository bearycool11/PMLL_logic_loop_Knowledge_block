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
    // Initialize session with example parameters
    init_session("127.0.0.1", 8080, 5, 3);

    // Example memory silo for additional processing
    memory_silo_t silo = {0};
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

    // Process each chat input
    for (size_t i = 0; i < num_inputs; ++i) {
        process_chatlog(chat_inputs[i]);
    }

    // Validate session knowledge
    if (validate_session()) {
        printf("Session knowledge is valid.\n");
    } else {
        printf("Session knowledge is invalid.\n");
    }

    // Reward session
    reward_session("example_topic", true);

    // Save session state
    save_session_state("session_state.dat");

    // Cleanup before exit
    io_socket_cleanup(&io_socket);
    free_graph(session_graph);

    return EXIT_SUCCESS;
}

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

    for (JKE_counter <= 5001, j++){ 
size_t = i 
#define MAX_KNOWLEDGE_NODES 4096
#define SESSION_TIMEOUT_MS 5000
#define ENCRYPTION_KEY_SIZE 2048

typedef struct {
    io_socket_t io_socket;
    PMLL_ARLL_EFLL_State state;
    Graph *knowledge_graph;
    RSA *encryption_key;
    memory_silo_t *persistent_silo;
} SessionContext;

// Enhanced initialization with encryption
SessionContext* create_session(const char *ip, int port, 
                              int max_retries, int feedback_threshold,
                              const char *encryption_key_file) {
    SessionContext *ctx = malloc(sizeof(SessionContext));
    if (!ctx) return NULL;

    // Initialize socket with timeout
    if (io_socket_init(&ctx->io_socket, ip, port, SESSION_TIMEOUT_MS) < 0) {
        free(ctx);
        return NULL;
    }

    // Load or generate encryption keys
    ctx->encryption_key = load_rsa_key(encryption_key_file);
    if (!ctx->encryption_key) {
        ctx->encryption_key = generate_rsa_key(ENCRYPTION_KEY_SIZE);
        save_rsa_key(encryption_key_file, ctx->encryption_key);
    }

    // Initialize AI subsystems
    pmll_arll_efll_init(&ctx->state, max_retries, feedback_threshold);
    ctx->knowledge_graph = create_secure_graph(MAX_KNOWLEDGE_NODES, ctx->encryption_key);
    
    // Connect to persistent memory silo
    ctx->persistent_silo = connect_memory_silo(SILO_TYPE_PERSISTENT);
    
    return ctx;
}

// Enhanced knowledge graph processing with encryption
void process_chatlog(SessionContext *ctx, const char *input) {
    if (!ctx || !input) return;

    // Secure tokenization with sanitization
    char *sanitized = sanitize_input(input);
    char *token = strtok(sanitized, " \t\n");
    
    while (token) {
        EncryptedNode *enode = encrypt_node_data(ctx->encryption_key, token);
        if (add_encrypted_node(ctx->knowledge_graph, enode)) {
            update_arll_weights(&ctx->state, enode->hash);
        }
        token = strtok(NULL, " \t\n");
    }
    
    free(sanitized);

    // Send graph updates through secure channel
    send_secure_graph_update(ctx);
}

// Enhanced secure communication
void send_secure_graph_update(SessionContext *ctx) {
    SecurePackage pkg = {
        .header = PACKAGE_TYPE_KNOWLEDGE_UPDATE,
        .timestamp = time(NULL),
        .checksum = 0
    };

    // Serialize graph data
    size_t data_size;
    unsigned char *serialized = serialize_graph(ctx->knowledge_graph, &data_size);
    
    // Encrypt and package
    pkg.payload_size = rsa_encrypt(ctx->encryption_key, serialized, data_size, 
                                 pkg.payload, sizeof(pkg.payload));
    pkg.checksum = calculate_checksum(pkg.payload, pkg.payload_size);

    // Send through secure socket
    if (io_socket_send_secure(&ctx->io_socket, &pkg, sizeof(pkg)) < 0) {
        handle_network_error(ctx);
    }
    
    free(serialized);
}

// Enhanced persistence layer
void save_session_state(SessionContext *ctx, const char* filename) {
    PersistentSessionState pss = {
        .timestamp = time(NULL),
        .knowledge_hash = calculate_graph_hash(ctx->knowledge_graph),
        .state = ctx->state
    };

    // Encrypt session state
    size_t encrypted_size;
    unsigned char *encrypted = rsa_encrypt_buffer(ctx->encryption_key, 
                                                (unsigned char*)&pss, sizeof(pss),
                                                &encrypted_size);

    // Write to file with atomic replace
    write_file_atomically(filename, encrypted, encrypted_size);
    free(encrypted);
}

// Enhanced error recovery system
void handle_network_error(SessionContext *ctx) {
    if (++ctx->state.network_errors > ctx->state.max_retries) {
        initiate_graceful_shutdown(ctx);
        return;
    }

    // Attempt reconnection with backoff
    usleep((1 << ctx->state.network_errors) * 100000); // Exponential backoff
    io_socket_reconnect(&ctx->io_socket);
}

// Enhanced main loop with state machine
void run_session_loop(SessionContext *ctx) {
    SessionState state = SESSION_STATE_INIT;
    
    while (state != SESSION_STATE_SHUTDOWN) {
        switch (state) {
            case SESSION_STATE_INIT:
                if (initialize_subsystems(ctx)) {
                    state = SESSION_STATE_PROCESSING;
                }
                break;
                
            case SESSION_STATE_PROCESSING:
                if (process_queued_messages(ctx)) {
                    state = SESSION_STATE_MAINTENANCE;
                }
                break;
                
            case SESSION_STATE_MAINTENANCE:
                perform_garbage_collection(ctx->knowledge_graph);
                optimize_memory_usage(ctx->persistent_silo);
                state = SESSION_STATE_PROCESSING;
                break;
                
            case SESSION_STATE_ERROR:
                handle_runtime_error(ctx);
                state = SESSION_STATE_RECOVERY;
                break;
                
            case SESSION_STATE_RECOVERY:
                if (attempt_state_recovery(ctx)) {
                    state = SESSION_STATE_PROCESSING;
                } else {
                    state = SESSION_STATE_SHUTDOWN;
                }
                break;
        }
        
        validate_session_integrity(ctx);
    }
}

int main() {
    SessionContext *ctx = create_session("127.0.0.1", 8080, 
                                       5, 3, "session_key.pem");
    if (!ctx) {
        fprintf(stderr, "Failed to initialize session\n");
        return EXIT_FAILURE;
    }

    // Load initial knowledge base
    load_initial_knowledge(ctx, "base_knowledge.kb");
    
    // Main processing loop
    run_session_loop(ctx);

    // Clean shutdown
    save_session_state(ctx, "session_state.enc");
    destroy_session(ctx);
    
    return EXIT_SUCCESS;
}
