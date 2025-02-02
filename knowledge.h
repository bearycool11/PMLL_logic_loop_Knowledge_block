#ifndef KNOWLEDGE_H
#define KNOWLEDGE_H

#include <stdint.h>
#include "memory_assoc.h"
#include "pml_logic_loop.h"

// Function prototypes for knowledge management

/**
 * Retrieve general knowledge graph data into a buffer.
 * @param buffer Buffer to store the knowledge graph data.
 * @param buffer_size Size of the buffer.
 */
void retrieve_general_knowledge(char* buffer, size_t buffer_size);

/**
 * Update the local knowledge graph with the provided data.
 * @param pml Pointer to the PMLL structure.
 * @param update_data JSON-like string containing the update data.
 */
void update_local_knowledge_graph(PMLL* pml, const char* update_data);

/**
 * Write a new node to the graph.
 * @param graph Pointer to the Graph structure.
 * @param entity Name of the entity.
 * @param hash Hash value associated with the node.
 */
void write_node_to_graph(Graph* graph, const char* entity, uint32_t hash);

/**
 * Write a new edge to the graph.
 * @param graph Pointer to the Graph structure.
 * @param from_entity Name of the starting node entity.
 * @param to_entity Name of the ending node entity.
 * @param relationship Relationship type between the nodes.
 * @param weight Weight of the edge.
 */
void write_edge_to_graph(Graph* graph, const char* from_entity, const char* to_entity, const char* relationship, uint32_t weight);

/**
 * Display the current state of the graph.
 * @param graph Pointer to the Graph structure.
 */
void display_graph(Graph* graph);

/**
 * Create and integrate a novel topic into the knowledge graph.
 * @param graph Pointer to the Graph structure.
 * @param topic Name of the novel topic.
 * @param related_entities Array of related entity names.
 * @param num_entities Number of related entities in the array.
 */
void integrate_novel_topic(Graph* graph, const char* topic, const char* related_entities[], uint32_t num_entities);

// Functions for specialized graphs

/**
 * Handle emotional graphs by managing emotional node data.
 * @param graph Pointer to the Graph structure.
 * @param emotion Name of the emotion node.
 * @param intensity Intensity of the emotion.
 */
void handle_emotional_graph(Graph* graph, const char* emotion, uint32_t intensity);

/**
 * Handle memory graphs by managing memory nodes and associations.
 * @param graph Pointer to the Graph structure.
 * @param memory_node Name of the memory node.
 * @param relevance Relevance score for the memory node.
 */
void handle_memory_graph(Graph* graph, const char* memory_node, uint32_t relevance);

/**
 * Handle mathematical graphs by managing equations and relationships.
 * @param graph Pointer to the Graph structure.
 * @param equation Mathematical equation to add as a node.
 */
void handle_math_graph(Graph* graph, const char* equation);

/**
 * Handle spatial graphs by managing spatial data and nodes.
 * @param graph Pointer to the Graph structure.
 * @param location Name of the spatial location.
 * @param coordinates Coordinates associated with the location.
 */
void handle_spatial_graph(Graph* graph, const char* location, const char* coordinates);

/**
 * Assign weights to nodes based on their category.
 * @param graph Pointer to the Graph structure.
 * @param node_name Name of the node.
 * @param weight Weight to assign to the node.
 */
void assign_node_weight(Graph* graph, const char* node_name, uint32_t weight);

#endif // KNOWLEDGE_H
