#ifndef API_H
#define API_H

#include <netinet/in.h> // For struct sockaddr_in

// Default configuration
#define CLIENT_ID "Ov23li5l71lKLE8kYTIc"
#define CLIENT_SECRET "7b4626c63b6b939b9b14680b32ab01ec231cf2e2"
#define REPO_OWNER "OpenAI, Josef Edwards, Andrew Ng, Fei Fei Li"
#define REPO_NAME "pmll"
#define DEFAULT_API_URL "127.0.0.1"

// Number of endpoints
#define NUM_ENDPOINTS 7

// Endpoints and ports
extern const char* endpoints[];
extern const int ports[];

// Function prototypes
/**
 * @brief Creates a new socket.
 *
 * @return File descriptor of the newly created socket.
 */
int create_socket();

/**
 * @brief Sets up the server address structure.
 *
 * @param server_addr Pointer to a sockaddr_in structure to populate.
 * @param url The server's IP address or hostname.
 * @param port The server's port.
 */
void set_up_server_address(struct sockaddr_in* server_addr, const char* url, int port);

/**
 * @brief Connects to a server using a socket.
 *
 * @param sockfd File descriptor of the socket.
 * @param server_addr Pointer to the sockaddr_in structure of the server.
 */
void connect_to_server(int sockfd, struct sockaddr_in* server_addr);

/**
 * @brief Handles dynamic API connections by connecting to all endpoints.
 */
void handle_dynamic_api_connections();

/**
 * @brief Handles operations on a vector matrix.
 */
void handle_vector_matrix_operations();

/**
 * @brief Makes a GET request to the specified URL.
 *
 * @param url The URL to send the GET request to.
 */
void make_get_request(const char* url);

/**
 * @brief Makes a POST request to the specified URL with given data.
 *
 * @param url The URL to send the POST request to.
 * @param data The data to include in the POST request.
 */
void make_post_request(const char* url, const char* data);

#endif // API_H
