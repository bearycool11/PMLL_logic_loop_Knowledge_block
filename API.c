#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "vector_Matrix.h" // Include the vector_Matrix header

#define CLIENT_ID "Ov23li5l71lKLE8kYTIc"
#define CLIENT_SECRET "7b4626c63b6b939b9b14680b32ab01ec231cf2e2"
#define REPO_OWNER "OpenAI, Josef Edwards, Andrew Ng, Fei Fei Li"
#define REPO_NAME "pmll"

// Default server configuration
#define DEFAULT_API_URL "127.0.0.1"
#define DEFAULT_API_PORT 8080

// Dynamic API endpoints and ports
const char* endpoints[] = {
    "VECTOR_MATRIX",
    "MEMORY_SILO",
    "IO_SOCKET",
    "LOGIC_LOOP",
    "PML_LOGIC_LOOP",
    "ARLL",
    "EFLL"
};

const int ports[] = {
    8080, // VECTOR_MATRIX
    8081, // MEMORY_SILO
    8082, // IO_SOCKET
    8083, // LOGIC_LOOP
    8084, // PML_LOGIC_LOOP
    8085, // ARLL
    8086  // EFLL
};

#define NUM_ENDPOINTS (sizeof(endpoints) / sizeof(endpoints[0]))

// Function to create a new socket
int create_socket() {
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("Failed to create socket");
        exit(EXIT_FAILURE);
    }
    return sockfd;
}

// Function to set up the server address
void set_up_server_address(struct sockaddr_in* server_addr, const char* url, int port) {
    memset(server_addr, 0, sizeof(*server_addr));
    server_addr->sin_family = AF_INET;
    server_addr->sin_port = htons(port);
    if (inet_pton(AF_INET, url, &server_addr->sin_addr) <= 0) {
        fprintf(stderr, "Invalid address: %s\n", url);
        exit(EXIT_FAILURE);
    }
}

// Function to connect to a server
void connect_to_server(int sockfd, struct sockaddr_in* server_addr) {
    if (connect(sockfd, (struct sockaddr*)server_addr, sizeof(*server_addr)) < 0) {
        perror("Failed to connect to server");
        close(sockfd);
        exit(EXIT_FAILURE);
    }
}

// Function to handle dynamic API connections
void handle_dynamic_api_connections() {
    struct sockaddr_in server_addr;
    int sockfd;

    for (int i = 0; i < NUM_ENDPOINTS; i++) {
        printf("Connecting to %s on port %d...\n", endpoints[i], ports[i]);
        sockfd = create_socket();
        set_up_server_address(&server_addr, DEFAULT_API_URL, ports[i]);
        connect_to_server(sockfd, &server_addr);

        // Send a basic request
        const char* request = "API_CONNECTION_TEST\n";
        if (send(sockfd, request, strlen(request), 0) < 0) {
            perror("Failed to send request");
        } else {
            printf("Request sent to %s successfully.\n", endpoints[i]);
        }

        // Receive a response
        char response[1024];
        if (recv(sockfd, response, sizeof(response), 0) < 0) {
            perror("Failed to receive response");
        } else {
            printf("Response from %s: %s\n", endpoints[i], response);
        }

        // Close the socket
        close(sockfd);
    }
}

// Function to perform vector matrix operations
void handle_vector_matrix_operations() {
    int rows = 3, cols = 3;
    int values[] = {1, 2, 3, 4, 5, 6, 7, 8, 9};

    vector_matrix_t* matrix = init_vector_matrix(rows, cols);
    if (!matrix) {
        fprintf(stderr, "Failed to initialize vector matrix\n");
        exit(EXIT_FAILURE);
    }

    if (populate_vector_matrix(matrix, values) != 0) {
        fprintf(stderr, "Failed to populate vector matrix\n");
        free_vector_matrix(matrix);
        exit(EXIT_FAILURE);
    }

    printf("Input Matrix:\n");
    for (int i = 0; i < matrix->rows; i++) {
        for (int j = 0; j < matrix->cols; j++) {
            printf("%d ", matrix->data[i][j]);
        }
        printf("\n");
    }

    if (pmll_vector_matrix_process(matrix) != 0) {
        fprintf(stderr, "Vector matrix processing failed\n");
        free_vector_matrix(matrix);
        exit(EXIT_FAILURE);
    }

    free_vector_matrix(matrix);
}

// Function to make a GET request
void make_get_request(const char* url) {
    CURL* curl = curl_easy_init();
    if (!curl) {
        fprintf(stderr, "Failed to initialize cURL\n");
        exit(EXIT_FAILURE);
    }

    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, NULL);

    CURLcode res = curl_easy_perform(curl);
    if (res != CURLE_OK) {
        fprintf(stderr, "cURL GET request failed: %s\n", curl_easy_strerror(res));
    }

    curl_easy_cleanup(curl);
}

// Function to make a POST request
void make_post_request(const char* url, const char* data) {
    CURL* curl = curl_easy_init();
    if (!curl) {
        fprintf(stderr, "Failed to initialize cURL\n");
        exit(EXIT_FAILURE);
    }

    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data);

    CURLcode res = curl_easy_perform(curl);
    if (res != CURLE_OK) {
        fprintf(stderr, "cURL POST request failed: %s\n", curl_easy_strerror(res));
    }

    curl_easy_cleanup(curl);
}

int main() {
    printf("Performing vector matrix operations...\n");
    handle_vector_matrix_operations();

    printf("Handling dynamic API connections...\n");
    handle_dynamic_api_connections();

    printf("Making a GET request to the GitHub API...\n");
    const char* get_url = "https://api.github.com/repos/OpenAI/pmll";
    make_get_request(get_url);

    printf("Making a POST request to create a new issue...\n");
    const char* post_data = "{\"title\":\"New issue\",\"body\":\"This is a new issue\"}";
    const char* post_url = "https://api.github.com/repos/OpenAI/pmll/issues";
    make_post_request(post_url, post_data);

    return 0;
}
