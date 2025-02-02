#ifndef MEMORY_ASSOC_H
#define MEMORY_ASSOC_H

#include <stdint.h>

// Define the structure for a node in the graph
typedef struct Node {
    char* entity;
    uint32_t hash;
    struct Node* next;
} Node;

// Define the structure for an edge in the graph
typedef struct Edge {
    Node* from;
    Node* to;
    char* relationship;
    uint32_t weight;
} Edge;

// Define the structure for the graph
typedef struct Graph {
    Node** nodes;
    Edge** edges;
    uint32_t num_nodes;
    uint32_t num_edges;
    uint32_t capacity_nodes;
    uint32_t capacity_edges;
} Graph;

// Define the memory association system
typedef struct MemoryAssoc {
    Graph* graph;
    uint32_t seed;
} MemoryAssoc;

// Function prototypes for Memory Association System
Node* create_node(const char* entity, uint32_t hash);
Edge* create_edge(Node* from, Node* to, const char* relationship, uint32_t weight);
Graph* create_graph(uint32_t initial_capacity);
void add_node(Graph* graph, Node* node);
void add_edge(Graph* graph, Edge* edge);
void get_relationships(Graph* graph, Node* entity);
MemoryAssoc* create_memory_assoc(uint32_t seed);
void add_entity(MemoryAssoc* memory_assoc, const char* entity, uint32_t hash);
void add_relationship(MemoryAssoc* memory_assoc, const char* entity1, const char* entity2, const char* relationship, uint32_t weight);
void get_relationships_memory_assoc(MemoryAssoc* memory_assoc, const char* entity);
void fetch_relationships_from_socket(MemoryAssoc* memory_assoc, const char* entity);
void cleanup_memory_assoc(MemoryAssoc* memory_assoc);

#endif // MEMORY_ASSOC_H
