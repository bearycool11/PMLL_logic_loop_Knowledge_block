#ifndef LOGIC_LOOPS_H
#define LOGIC_LOOPS_H

#include <stdbool.h>

// Constants
#define MAX_ITERATIONS 100
#define BUFFER_SIZE 1024
#define RETRY_LIMIT 5

// API Endpoints
#define API_ENDPOINT_CHATGPT "http://127.0.0.1:8080/api/chatgpt"
#define API_ENDPOINT_LLAMA "http://127.0.0.1:8081/api/llama"

// Error Flags for EFLL
typedef enum {
    EFLL_ERROR_NONE,
    EFLL_ERROR_MEMORY,
    EFLL_ERROR_IO,
    EFLL_ERROR_API,
    EFLL_ERROR_UNKNOWN
} efll_error_t;

// PMLL Structure
typedef struct {
    int id;
    int memory_silo_id;
    int io_socket_id;
    int free_c_present;
    int retries;      // Retry counter for ARLL
    efll_error_t error_flag;  // Error flag for EFLL

    // Function pointers for modular operations
    void (*monitor)(struct PMLL*);
    void (*evaluate)(struct PMLL*);
    void (*automate)(struct PMLL*);
} PMLL;

/**
 * Initialize the PML Logic Loop.
 * @param pml Pointer to the PMLL structure.
 * @return 0 on success, -1 on error.
 */
int initialize_logic_loop(PMLL* pml);

/**
 * Execute the PML Logic Loop.
 * @param pml Pointer to the PMLL structure.
 */
void execute_logic_loop(PMLL* pml);

/**
 * Cleanup the PML Logic Loop resources.
 * @param pml Pointer to the PMLL structure.
 */
void cleanup_logic_loop(PMLL* pml);

/**
 * Consolidate memory across silos.
 * @param pml Pointer to the PMLL structure.
 * @param pipefd Pipe file descriptors for inter-process communication.
 */
void consolidate_memory(PMLL* pml, int pipefd[2]);

/**
 * Update consolidated memory graph.
 * @param pml Pointer to the PMLL structure.
 * @param buffer Data for updating the memory graph.
 */
void update_consolidated_memory_graph(PMLL* pml, const char* buffer);

/**
 * Save PML metrics to persistent storage.
 * @param pml Pointer to the PMLL structure.
 */
void save_pml_metrics(PMLL* pml);

/**
 * Load PML metrics from persistent storage.
 * @param pml Pointer to the PMLL structure.
 */
void load_pml_metrics(PMLL* pml);

/**
 * Send an API request and receive the response.
 * @param endpoint The API endpoint URL.
 * @param request The request payload.
 * @param response Buffer to store the response.
 * @param response_size Size of the response buffer.
 * @return 0 on success, -1 on error.
 */
int send_api_request(const char* endpoint, const char* request, char* response, size_t response_size);

#endif // LOGIC_LOOPS_H
