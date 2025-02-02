// io_socket.h
#ifndef IO_SOCKET_H
#define IO_SOCKET_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define BUFFER_SIZE 1024

// Function prototypes
int create_io_socket(const char* ip, int port);
void io_socket_write(int socket_fd, const char* data, size_t size);
void io_socket_read(int socket_fd, char* buffer, size_t size);
void io_socket_cleanup(int socket_fd);

#endif // IO_SOCKET_H

// io_socket.c
#include "io_socket.h"

int create_io_socket(const char* ip, int port) {
    int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_fd < 0) {
        perror("Socket creation failed");
        return -1;
    }

    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);

    if (inet_pton(AF_INET, ip, &server_addr.sin_addr) <= 0) {
        perror("Invalid address");
        close(socket_fd);
        return -1;
    }

    if (connect(socket_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("Connection failed");
        close(socket_fd);
        return -1;
    }

    return socket_fd;
}

void io_socket_write(int socket_fd, const char* data, size_t size) {
    if (write(socket_fd, data, size) < 0) {
        perror("Socket write failed");
    }
}

void io_socket_read(int socket_fd, char* buffer, size_t size) {
    if (read(socket_fd, buffer, size) < 0) {
        perror("Socket read failed");
    }
}

void io_socket_cleanup(int socket_fd) {
    close(socket_fd);
}

// memory_assoc.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "io_socket.h"

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

// Function prototypes
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

// Function to fetch relationships from the socket
void fetch_relationships_from_socket(MemoryAssoc* memory_assoc, const char* entity) {
    int socket_fd = create_io_socket("127.0.0.1", 8080); // Replace with your actual server details
    if (socket_fd < 0) {
        fprintf(stderr, "Failed to connect to the relationship server.\n");
        return;
    }

    // Send request to fetch relationships
    char request[BUFFER_SIZE];
    snprintf(request, BUFFER_SIZE, "GET_RELATIONSHIPS %s\n", entity);
    io_socket_write(socket_fd, request, strlen(request));

    // Receive response from the server
    char response[BUFFER_SIZE];
    io_socket_read(socket_fd, response, sizeof(response));

    printf("Server Response: %s\n", response);

    // Parse response and update the graph
    char* line = strtok(response, "\n");
    while (line != NULL) {
        char entity1[256], entity2[256], relationship[256];
        uint32_t weight;

        if (sscanf(line, "%s %s %s %u", entity1, entity2, relationship, &weight) == 4) {
            add_relationship(memory_assoc, entity1, entity2, relationship, weight);
        }
        line = strtok(NULL, "\n");
    }

    // Cleanup
    io_socket_cleanup(socket_fd);
}

// Updated function to add a relationship with socket integration
void add_relationship(MemoryAssoc* memory_assoc, const char* entity1, const char* entity2, const char* relationship, uint32_t weight) {
    Node* node1 = NULL;
    Node* node2 = NULL;

    // Find existing nodes or create new ones
    for (uint32_t i = 0; i < memory_assoc->graph->num_nodes; i++) {
        if (strcmp(memory_assoc->graph->nodes[i]->entity, entity1) == 0) {
            node1 = memory_assoc->graph->nodes[i];
        }
        if (strcmp(memory_assoc->graph->nodes[i]->entity, entity2) == 0) {
            node2 = memory_assoc->graph->nodes[i];
        }
    }

    // Create nodes if they don't exist
    if (!node1) {
        node1 = create_node(entity1, 0);
        add_node(memory_assoc->graph, node1);
    }
    if (!node2) {
        node2 = create_node(entity2, 0);
        add_node(memory_assoc->graph, node2);
    }

    Edge* edge = create_edge(node1, node2, relationship, weight);
    add_edge(memory_assoc->graph, edge);

    // Log the relationship addition
    printf("Added relationship: %s -> %s (%s, %u)\n", entity1, entity2, relationship, weight);
}

// Example usage
int main() {
    MemoryAssoc* memory_assoc = create_memory_assoc(42);

    // Add entities and relationships
    add_entity(memory_assoc, "Entity1", 1);
    add_entity(memory_assoc, "Entity2", 2);
    add_relationship(memory_assoc, "Entity1", "Entity2", "Relationship", 3);

    // Fetch additional relationships dynamically
    fetch_relationships_from_socket(memory_assoc, "Entity1");

    // Display relationships
    get_relationships_memory_assoc(memory_assoc, "Entity1");

    // Cleanup
    cleanup_memory_assoc(memory_assoc);
    return 0;
}
