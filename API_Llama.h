#ifndef API_H
#define API_H

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

// Constants
#define API_URL "localhost"
#define API_PORT 8080
#define CROSS_TALK_URL "localhost"
#define CROSS_TALK_PORT 8081
#define PMLL_URL "localhost"
#define PMLL_PORT 8082
#define ARLL_URL "localhost"
#define ARLL_PORT 8083
#define EFLL_URL "localhost"
#define EFLL_PORT 8084

#define BUFFER_SIZE 1024

// Function Prototypes

// Socket management
int create_socket();
void set_up_server_address(struct sockaddr_in* server_addr, const char* url, int port);
void connect_to_server(int sockfd, struct sockaddr_in* server_addr);

// Request and response handling
void send_request(int sockfd, const char* request);
void receive_response(int sockfd, char* response);

// Logic loop cascading
void run_logic_loop_cascade(int sockfd, const char* request);

#endif // API_Ho
