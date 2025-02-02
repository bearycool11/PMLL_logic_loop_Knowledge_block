#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <jansson.h> // JSON parsing library
#include <arpa/inet.h>
#include <errno.h>
#include <time.h>

#define BUFFER_SIZE 1024
#define CHUNK_SIZE 256
#define RETRY_LIMIT 5

// Define the buffer and chunk
char buffer[BUFFER_SIZE];
char chunk[CHUNK_SIZE];

// Define the JSON object and error variable
json_t* json;
json_error_t json_error;

// Function prototypes
int setup_server_socket();
int handle_client_connection(int client_sock);
void write_to_knowledge_graph(json_t* json);
void pmll_arll_efll_logic(int client_sock);
void log_message(const char* level, const char* message);
void cleanup();

void pmll_arll_efll_logic(int client_sock) {
    int retry_count = 0;
    int success = 0;

    // PMLL: Persistent Memory Logic Loop
    while (retry_count < RETRY_LIMIT) {
        memset(buffer, 0, sizeof(buffer)); // Clear the buffer for each retry
        printf("\n[PMLL] Listening for incoming data (attempt %d)...\n", retry_count + 1);

        int bytes_received;
        while ((bytes_received = recv(client_sock, chunk, CHUNK_SIZE, 0)) > 0) {
            chunk[bytes_received] = '\0';
            strcat(buffer, chunk);

            // Attempt to parse JSON
            json = json_loads(buffer, 0, &json_error);
            if (!json) {
                log_message("ERROR", json_error.text);
                continue; // Wait for more data
            }

            // ARLL: Adaptive Redundancy Logic Loop
            printf("\n[ARLL] Valid JSON received. Writing to knowledge graph...\n");
            write_to_knowledge_graph(json);
            success = 1;
            break; // Exit the retry loop if successful
        }

        if (success) {
            printf("[PMLL] Data processed successfully.\n");
            break; // Exit the retry loop
        }

        log_message("WARNING", "[EFLL] Error in processing. Retrying...");
        retry_count++;
        usleep(100000); // Sleep before retrying
    }

    if (!success) {
        log_message("FATAL", "[EFLL] Maximum retries reached. Exiting...");
    }
}

int setup_server_socket() {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(8080);
    inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr);

    if (bind(sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind failed");
        close(sock);
        exit(EXIT_FAILURE);
    }

    if (listen(sock, 1) < 0) {
        perror("Listen failed");
        close(sock);
        exit(EXIT_FAILURE);
    }

    printf("[INFO] Server listening on port 8080...\n");
    return sock;
}

int handle_client_connection(int client_sock) {
    pmll_arll_efll_logic(client_sock);
    return 0;
}

void write_to_knowledge_graph(json_t* json) {
    if (!json) {
        log_message("ERROR", "Invalid JSON data for knowledge graph.");
        return;
    }

    printf("[ARLL] Writing JSON data to knowledge graph...\n");
    // Simulate writing JSON to a knowledge graph
    json_dumpf(json, stdout, JSON_INDENT(2));
    printf("\n[ARLL] Data successfully written to knowledge graph.\n");
}

void log_message(const char* level, const char* message) {
    time_t now = time(NULL);
    char timestamp[64];
    strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", localtime(&now));
    printf("[%s] [%s] %s\n", level, timestamp, message);
}

void cleanup() {
    if (json) {
        json_decref(json);
        json = NULL;
    }
    memset(buffer, 0, sizeof(buffer));
}

int main() {
    int server_sock = setup_server_socket();

    while (1) {
        printf("[INFO] Waiting for a client to connect...\n");
        int client_sock = accept(server_sock, NULL, NULL);
        if (client_sock < 0) {
            perror("Accept failed");
            continue;
        }

        printf("[INFO] Client connected.\n");

        // Handle client connection
        handle_client_connection(client_sock);

        // Cleanup after each client
        cleanup();
        close(client_sock);
        printf("[INFO] Client connection closed.\n");
    }

    close(server_sock);
    return 0;
}
