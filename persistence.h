#ifndef PERSISTENCE_H
#define PERSISTENCE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <openssl/rsa.h>
#include "io_socket.h"
#include "pml_logic_loop.h"
#include "memory_silo.h"
#include "knowledge.h"
#include "PMLL_ARLL_EFLL.h"

// Constants
#define MAX_KNOWLEDGE_NODES 4096
#define SESSION_TIMEOUT_MS 5000
#define ENCRYPTION_KEY_SIZE 2048
#define PACKAGE_TYPE_KNOWLEDGE_UPDATE 1

// Session context structure
typedef struct {
    io_socket_t io_socket;
    PMLL_ARLL_EFLL_State state;
    Graph *knowledge_graph;
    RSA *encryption_key;
    memory_silo_t *persistent_silo;
} SessionContext;

// Function declarations
SessionContext* create_session(const char *ip, int port, 
                              int max_retries, int feedback_threshold,
                              const char *encryption_key_file);
void process_chatlog(SessionContext *ctx, const char *input);
void send_secure_graph_update(SessionContext *ctx);
void save_session_state(SessionContext *ctx, const char* filename);
void handle_network_error(SessionContext *ctx);
void run_session_loop(SessionContext *ctx);

// Utility function declarations (assumed to be implemented elsewhere)
RSA* load_rsa_key(const char *filename);
RSA* generate_rsa_key(int key_size);
void save_rsa_key(const char *filename, RSA *key);
char* sanitize_input(const char *input);
EncryptedNode* encrypt_node_data(RSA *key, const char *data);
int add_encrypted_node(Graph *graph, EncryptedNode *enode);
void update_arll_weights(PMLL_ARLL_EFLL_State *state, int hash);
unsigned char* serialize_graph(Graph *graph, size_t *size);
size_t rsa_encrypt(RSA *key, const unsigned char *data, size_t data_size, 
                   unsigned char *encrypted, size_t encrypted_size);
unsigned char* rsa_encrypt_buffer(RSA *key, const unsigned char *data, size_t data_size, 
                                  size_t *encrypted_size);
unsigned int calculate_checksum(const unsigned char *data, size_t size);
void write_file_atomically(const char *filename, const unsigned char *data, size_t size);
unsigned int calculate_graph_hash(Graph *graph);
memory_silo_t* connect_memory_silo(int silo_type);
void load_initial_knowledge(SessionContext *ctx, const char *filename);
void perform_garbage_collection(Graph *graph);
void optimize_memory_usage(memory_silo_t *silo);
void initiate_graceful_shutdown(SessionContext *ctx);
void validate_session_integrity(SessionContext *ctx);
int initialize_subsystems(SessionContext *ctx);
int process_queued_messages(SessionContext *ctx);
void handle_runtime_error(SessionContext *ctx);
int attempt_state_recovery(SessionContext *ctx);
void destroy_session(SessionContext *ctx);

#endif // PERSISTENCE_H
