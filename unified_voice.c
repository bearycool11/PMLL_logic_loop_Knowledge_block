#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "io_socket.h"
#include "memory_silo.h"
#include "knowledge.h"
#include "pml_logic_loop.h"
#include "utf8_tokenizer.h"

// Define a unified voice structure
typedef struct {
    io_socket_t io_socket;
    memory_silo_t *memory_silo;
    pml_logic_loop_t *pml_logic_loop;
    Graph *knowledge_graph;
} unified_voice_t;

// Function to initialize the unified voice system
unified_voice_t* init_unified_voice(const char *ip, int port, int silo_id) {
    unified_voice_t *uv = malloc(sizeof(unified_voice_t));
    if (!uv) {
        perror("Failed to allocate unified voice instance");
        exit(EXIT_FAILURE);
    }

    // Initialize IO socket
    if (io_socket_init(&uv->io_socket, ip, port) < 0) {
        fprintf(stderr, "Failed to initialize IO socket\n");
        free(uv);
        return NULL;
    }

    // Initialize memory silo
    uv->memory_silo = memory_silo_init(silo_id);
    if (!uv->memory_silo) {
        fprintf(stderr, "Failed to initialize memory silo\n");
        io_socket_cleanup(&uv->io_socket);
        free(uv);
        return NULL;
    }

    // Initialize PML logic loop
    uv->pml_logic_loop = pml_logic_loop_init(silo_id, uv->io_socket.socket);
    if (!uv->pml_logic_loop) {
        fprintf(stderr, "Failed to initialize PML logic loop\n");
        memory_silo_free(uv->memory_silo);
        io_socket_cleanup(&uv->io_socket);
        free(uv);
        return NULL;
    }

    // Initialize knowledge graph
    uv->knowledge_graph = create_graph(1024);
    if (!uv->knowledge_graph) {
        fprintf(stderr, "Failed to initialize knowledge graph\n");
        pml_logic_loop_cleanup(uv->pml_logic_loop);
        memory_silo_free(uv->memory_silo);
        io_socket_cleanup(&uv->io_socket);
        free(uv);
        return NULL;
    }

    return uv;
}

// Function to process UTF-8 input and update knowledge graph
void process_utf8_and_update_knowledge(unified_voice_t *uv, const char *input) {
    if (!uv || !input) {
        fprintf(stderr, "Invalid parameters for process_utf8_and_update_knowledge\n");
        return;
    }

    printf("Processing input: \"%s\"\n", input);

    const char *delimiters = " \t\n";
    const char *start = input;
    const char *current = input;

    while (*current) {
        if (strchr(delimiters, *current) != NULL) {
            if (current > start) {
                // Extract token
                size_t token_len = current - start;
                char *token = strndup(start, token_len);

                // Add token to the knowledge graph
                Node *node = create_node(token, 0);
                add_node(uv->knowledge_graph, node);
                printf("Added token to knowledge graph: %s\n", token);

                free(token);
            }
            current++;
            start = current;
        } else {
            if (is_utf8_start_byte((unsigned char)*current)) {
                current++;
                while (*current && (*current & 0xC0) == 0x80) {
                    current++;
                }
            } else {
                current++;
            }
        }
    }

    if (current > start) {
        // Add last token
        size_t token_len = current - start;
        char *token = strndup(start, token_len);

        Node *node = create_node(token, 0);
        add_node(uv->knowledge_graph, node);
        printf("Added token to knowledge graph: %s\n", token);

        free(token);
    }
}

// Function to handle cross-talk communication
void cross_talk(unified_voice_t *uv, const char *message) {
    if (!uv || !message) {
        fprintf(stderr, "Invalid parameters for cross-talk\n");
        return;
    }

    printf("Sending message to cross-talk socket: %s\n", message);
    io_socket_write(uv->io_socket.socket, message, strlen(message));

    char buffer[1024];
    io_socket_read(uv->io_socket.socket, buffer, sizeof(buffer));
    printf("Received response: %s\n", buffer);

    // Update the knowledge graph with the response
    process_utf8_and_update_knowledge(uv, buffer);
}

// Function to run the unified voice system
void run_unified_voice(unified_voice_t *uv) {
    if (!uv) {
        fprintf(stderr, "Unified voice instance is NULL\n");
        return;
    }

    const char *test_input = "This is a test input for cross-talk";
    process_utf8_and_update_knowledge(uv, test_input);

    cross_talk(uv, "Hello from Unified Voice");

    // Simulate PML logic loop execution
    printf("Executing PML logic loop...\n");
    pml_logic_loop_process(&uv->io_socket);

    printf("Unified voice processing completed.\n");
}

// Cleanup function for the unified voice system
void cleanup_unified_voice(unified_voice_t *uv) {
    if (!uv) return;

    pml_logic_loop_cleanup(uv->pml_logic_loop);
    memory_silo_free(uv->memory_silo);
    destroy_graph(uv->knowledge_graph);
    io_socket_cleanup(&uv->io_socket);
    free(uv);
}

// Main function
int main() {
    unified_voice_t *uv = init_unified_voice("127.0.0.1", 8080, 1);
    if (!uv) {
        fprintf(stderr, "Failed to initialize unified voice system\n");
        return EXIT_FAILURE;
    }

    run_unified_voice(uv);
    cleanup_unified_voice(uv);

    return EXIT_SUCCESS;
}

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "io_socket.h"
#include "memory_silo.h"
#include "knowledge.h"
#include "pml_logic_loop.h"
#include "utf8_tokenizer.h"

// Define tree structure for LTM
typedef struct TreeNode {
    char *data;
    struct TreeNode *parent;
    struct TreeNode *children[10];
    int child_count;
} TreeNode;

// Define unified voice structure
typedef struct {
    io_socket_t io_socket;
    memory_silo_t *memory_silo;
    pml_logic_loop_t *pml_logic_loop;
    Graph *knowledge_graph;
    bool efll_flag;
} unified_voice_t;

// ARLL rewards structure
typedef struct {
    int good_true_rewards;
    int false_good_rewards;
} ARLLRewards;

// Function to create a tree node for LTM
TreeNode* create_ltm_node(const char *data, TreeNode *parent) {
    TreeNode *node = malloc(sizeof(TreeNode));
    node->data = strdup(data);
    node->parent = parent;
    node->child_count = 0;

    if (parent && parent->child_count < 10) {
        parent->children[parent->child_count++] = node;
    }

    return node;
}

// Function to add a node to the LTM tree hierarchy
void add_to_ltm(TreeNode *root, const char *data) {
    if (!root) return;

    TreeNode *current = root;

    // Traverse to find the right insertion point
    while (current->child_count > 0) {
        current = current->children[0]; // Traverse down the tree
    }

    create_ltm_node(data, current);
    printf("Added to LTM: %s\n", data);
}

// Function to perform a mimeograph rollout from STM to LTM
void mimeograph_rollout(Graph *stm, TreeNode *ltm_root) {
    if (!stm || !ltm_root) return;

    printf("Starting Mimeograph Rollout...\n");

    for (int i = 0; i < stm->node_count; ++i) {
        Node *node = stm->nodes[i];
        add_to_ltm(ltm_root, node->data);
    }

    printf("Mimeograph Rollout completed.\n");
}

// EFLL judge function to validate memory
bool efll_judge_memory(Graph *knowledge_graph) {
    if (knowledge_graph->node_count < 10) {
        printf("EFLL: Memory flagged as incomplete or invalid.\n");
        return false;
    }

    printf("EFLL: Memory flagged as valid.\n");
    return true;
}

// ARLL reward function for novel topics
void arll_reward_memory(const char *topic, ARLLRewards *rewards, bool is_good) {
    if (is_good) {
        printf("ARLL: Rewarding topic '%s' as Good and True.\n", topic);
        rewards->good_true_rewards++;
    } else {
        printf("ARLL: Rewarding topic '%s' as False but Good.\n", topic);
        rewards->false_good_rewards++;
    }
}

// Function to initialize the unified voice system
unified_voice_t* init_unified_voice(const char *ip, int port, int silo_id) {
    unified_voice_t *uv = malloc(sizeof(unified_voice_t));
    if (!uv) {
        perror("Failed to allocate unified voice instance");
        exit(EXIT_FAILURE);
    }

    // Initialize IO socket
    if (io_socket_init(&uv->io_socket, ip, port) < 0) {
        fprintf(stderr, "Failed to initialize IO socket\n");
        free(uv);
        return NULL;
    }

    // Initialize memory silo
    uv->memory_silo = memory_silo_init(silo_id);
    if (!uv->memory_silo) {
        fprintf(stderr, "Failed to initialize memory silo\n");
        io_socket_cleanup(&uv->io_socket);
        free(uv);
        return NULL;
    }

    // Initialize PML logic loop
    uv->pml_logic_loop = pml_logic_loop_init(silo_id, uv->io_socket.socket);
    if (!uv->pml_logic_loop) {
        fprintf(stderr, "Failed to initialize PML logic loop\n");
        memory_silo_free(uv->memory_silo);
        io_socket_cleanup(&uv->io_socket);
        free(uv);
        return NULL;
    }

    // Initialize knowledge graph
    uv->knowledge_graph = create_graph(1024);
    if (!uv->knowledge_graph) {
        fprintf(stderr, "Failed to initialize knowledge graph\n");
        pml_logic_loop_cleanup(uv->pml_logic_loop);
        memory_silo_free(uv->memory_silo);
        io_socket_cleanup(&uv->io_socket);
        free(uv);
        return NULL;
    }

    uv->efll_flag = true; // Assume memory is valid initially
    return uv;
}

// Function to process input and update STM
void process_utf8_and_update_knowledge(unified_voice_t *uv, const char *input) {
    if (!uv || !input) {
        fprintf(stderr, "Invalid parameters for processing input.\n");
        return;
    }

    printf("Processing input: %s\n", input);

    char *token = strtok(strdup(input), " \t\n");
    while (token) {
        Node *node = create_node(token, 0);
        add_node(uv->knowledge_graph, node);
        printf("Added to STM: %s\n", token);
        token = strtok(NULL, " \t\n");
    }
}

// Function to handle cross-talk communication
void cross_talk(unified_voice_t *uv, const char *message) {
    if (!uv || !message) {
        fprintf(stderr, "Invalid parameters for cross-talk.\n");
        return;
    }

    printf("Sending message: %s\n", message);
    io_socket_write(uv->io_socket.socket, message, strlen(message));

    char buffer[1024];
    io_socket_read(uv->io_socket.socket, buffer, sizeof(buffer));
    printf("Received response: %s\n", buffer);

    // Update STM with response
    process_utf8_and_update_knowledge(uv, buffer);
}

// Main processing loop
void run_unified_voice(unified_voice_t *uv) {
    if (!uv) {
        fprintf(stderr, "Unified voice instance is NULL.\n");
        return;
    }

    const char *test_input = "Sample input for Unified Voice system";
    process_utf8_and_update_knowledge(uv, test_input);

    cross_talk(uv, "Requesting cross-talk message.");

    uv->efll_flag = efll_judge_memory(uv->knowledge_graph);
    if (uv->efll_flag) {
        ARLLRewards rewards = {0, 0};
        arll_reward_memory("Sample Topic", &rewards, true);
    }

    TreeNode *ltm_root = create_ltm_node("Root", NULL);
    mimeograph_rollout(uv->knowledge_graph, ltm_root);

    printf("Unified voice processing completed.\n");
}

// Cleanup function for the unified voice system
void cleanup_unified_voice(unified_voice_t *uv) {
    if (!uv) return;

    pml_logic_loop_cleanup(uv->pml_logic_loop);
    memory_silo_free(uv->memory_silo);
    destroy_graph(uv->knowledge_graph);
    io_socket_cleanup(&uv->io_socket);
    free(uv);
}

// Main function
int main() {
    unified_voice_t *uv = init_unified_voice("127.0.0.1", 8080, 1);
    if (!uv) {
        fprintf(stderr, "Failed to initialize Unified Voice system.\n");
        return EXIT_FAILURE;
    }

    run_unified_voice(uv);
    cleanup_unified_voice(uv);

    return EXIT_SUCCESS;
}
