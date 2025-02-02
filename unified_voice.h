#ifndef UNIFIED_VOICE_H
#define UNIFIED_VOICE_H

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
    bool efll_flag; 
} unified_voice_t;

// ARLL rewards structure
typedef struct {
    int good_true_rewards;
    int false_good_rewards;
} ARLLRewards;

// Tree structure for LTM
typedef struct TreeNode {
    char *data;
    struct TreeNode *parent;
    struct TreeNode *children[10];
    int child_count;
} TreeNode;

// Function prototypes
unified_voice_t* init_unified_voice(const char *ip, int port, int silo_id);
void process_utf8_and_update_knowledge(unified_voice_t *uv, const char *input);
void cross_talk(unified_voice_t *uv, const char *message);
void run_unified_voice(unified_voice_t *uv);
void cleanup_unified_voice(unified_voice_t *uv);

// Additional functions
TreeNode* create_ltm_node(const char *data, TreeNode *parent);
void add_to_ltm(TreeNode *root, const char *data);
void mimeograph_rollout(Graph *stm, TreeNode *ltm_root);
bool efll_judge_memory(Graph *knowledge_graph);
void arll_reward_memory(const char *topic, ARLLRewards *rewards, bool is_good);

#endif // UNIFIED_VOICE_H
