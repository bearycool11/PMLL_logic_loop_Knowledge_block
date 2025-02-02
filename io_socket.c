#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <time.h>
#include <libwebsockets.h>
#include "logging.h"
#include "knowledge.h"
#include "memory_silo.h"
#include "logic_loop.h"

#define BUFFER_SIZE 2048
#define LOG_FILE "io_socket.log"
#define RETRY_LIMIT 5
#define WEBSOCKET_SERVER_URL "ws://127.0.0.1:8080"

// Unified IO type
typedef enum {
    IO_TYPE_TCP,
    IO_TYPE_WEBSOCKET
} io_type_t;

// Unified IO structure
typedef struct {
    io_type_t type;
    int tcp_socket;                 // For TCP/IP
    struct lws *websocket;          // For WebSocket
    struct lws_context *ws_context; // WebSocket context
} unified_io_t;

// Logging utility
void log_message(const char *level, const char *message) {
    FILE *log_file = fopen(LOG_FILE, "a");
    if (log_file) {
        time_t now = time(NULL);
        char *timestamp = ctime(&now);
        timestamp[strlen(timestamp) - 1] = '\0'; // Remove newline
        fprintf(log_file, "[%s] [%s] %s\n", level, timestamp, message);
        fclose(log_file);
    } else {
        fprintf(stderr, "Failed to open log file: %s\n", LOG_FILE);
    }
}

// TCP/IP Functions
int init_tcp_socket(const char *ip, int port) {
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        log_message("ERROR", "Failed to create TCP socket");
        perror("Error creating TCP socket");
        return -1;
    }

    struct sockaddr_in server_addr = {0};
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);

    if (inet_pton(AF_INET, ip, &server_addr.sin_addr) <= 0) {
        log_message("ERROR", "Invalid IP address");
        perror("Invalid IP address");
        close(sockfd);
        return -1;
    }

    if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        log_message("ERROR", "Failed to connect to TCP server");
        perror("Error connecting to TCP server");
        close(sockfd);
        return -1;
    }

    log_message("INFO", "Connected to TCP server");
    return sockfd;
}

int tcp_send(int socket, const char *message) {
    return send(socket, message, strlen(message), 0);
}

int tcp_receive(int socket, char *buffer, size_t buffer_size) {
    ssize_t bytes_received = recv(socket, buffer, buffer_size - 1, 0);
    if (bytes_received >= 0) {
        buffer[bytes_received] = '\0'; // Null-terminate
    }
    return bytes_received;
}

// WebSocket Functions
static int websocket_callback(struct lws *wsi, enum lws_callback_reasons reason, void *user, void *in, size_t len) {
    switch (reason) {
        case LWS_CALLBACK_CLIENT_ESTABLISHED:
            log_message("INFO", "WebSocket connection established.");
            break;

        case LWS_CALLBACK_CLIENT_RECEIVE:
            log_message("INFO", "Message received from WebSocket server.");
            printf("Received: %.*s\n", (int)len, (char *)in);
            break;

        case LWS_CALLBACK_CLIENT_WRITEABLE:
            log_message("INFO", "WebSocket is writable.");
            break;

        case LWS_CALLBACK_CLOSED:
            log_message("INFO", "WebSocket connection closed.");
            break;

        case LWS_CALLBACK_CLIENT_CONNECTION_ERROR:
            log_message("ERROR", "WebSocket connection error.");
            break;

        default:
            break;
    }
    return 0;
}

struct lws_context *init_websocket_context() {
    struct lws_context_creation_info info = {0};
    info.port = CONTEXT_PORT_NO_LISTEN;
    info.protocols = (struct lws_protocols[]){
        {"ws-protocol", websocket_callback, 0, BUFFER_SIZE},
        {NULL, NULL, 0, 0}};
    return lws_create_context(&info);
}

struct lws *connect_websocket(struct lws_context *ctx) {
    struct lws_client_connect_info connect_info = {0};
    connect_info.context = ctx;
    connect_info.address = "127.0.0.1";
    connect_info.port = 8080;
    connect_info.path = "/";
    connect_info.host = lws_canonical_hostname(ctx);
    connect_info.origin = "origin";
    connect_info.protocol = "ws-protocol";

    return lws_client_connect_via_info(&connect_info);
}

void websocket_send(struct lws *wsi, const char *message) {
    unsigned char buf[LWS_SEND_BUFFER_PRE_PADDING + BUFFER_SIZE + LWS_SEND_BUFFER_POST_PADDING];
    unsigned char *msg = &buf[LWS_SEND_BUFFER_PRE_PADDING];
    size_t len = strlen(message);
    memcpy(msg, message, len);
    lws_write(wsi, msg, len, LWS_WRITE_TEXT);
}

// Unified IO Functions
unified_io_t *init_unified_io(io_type_t type, const char *address, int port) {
    unified_io_t *io = malloc(sizeof(unified_io_t));
    if (!io) {
        log_message("ERROR", "Failed to allocate memory for unified IO");
        return NULL;
    }

    io->type = type;

    if (type == IO_TYPE_TCP) {
        io->tcp_socket = init_tcp_socket(address, port);
        if (io->tcp_socket < 0) {
            free(io);
            return NULL;
        }
    } else if (type == IO_TYPE_WEBSOCKET) {
        io->ws_context = init_websocket_context();
        if (!io->ws_context) {
            free(io);
            return NULL;
        }

        io->websocket = connect_websocket(io->ws_context);
        if (!io->websocket) {
            lws_context_destroy(io->ws_context);
            free(io);
            return NULL;
        }
    }

    return io;
}

void send_message(unified_io_t *io, const char *message) {
    if (!io || !message) return;

    if (io->type == IO_TYPE_TCP) {
        tcp_send(io->tcp_socket, message);
    } else if (io->type == IO_TYPE_WEBSOCKET) {
        websocket_send(io->websocket, message);
    }
}

void receive_message(unified_io_t *io, char *buffer, size_t buffer_size) {
    if (!io || !buffer) return;

    if (io->type == IO_TYPE_TCP) {
        tcp_receive(io->tcp_socket, buffer, buffer_size);
    }
}

void cleanup_unified_io(unified_io_t *io) {
    if (!io) return;

    if (io->type == IO_TYPE_TCP) {
        close(io->tcp_socket);
    } else if (io->type == IO_TYPE_WEBSOCKET) {
        lws_context_destroy(io->ws_context);
    }

    free(io);
}

// Main Function
int main() {
    unified_io_t *io_tcp = init_unified_io(IO_TYPE_TCP, "127.0.0.1", 8080);
    unified_io_t *io_ws = init_unified_io(IO_TYPE_WEBSOCKET, "127.0.0.1", 8080);

    if (io_tcp) {
        send_message(io_tcp, "Hello TCP Server!");
        char buffer[BUFFER_SIZE];
        receive_message(io_tcp, buffer, sizeof(buffer));
        printf("TCP Response: %s\n", buffer);
        cleanup_unified_io(io_tcp);
    }

    if (io_ws) {
        send_message(io_ws, "Hello WebSocket Server!");
        // WebSocket processing is handled asynchronously
        cleanup_unified_io(io_ws);
    }

    return 0;
}
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <time.h>
#include <libwebsockets.h>

#define BUFFER_SIZE 2048
#define LOG_FILE "io_socket.log"
#define RETRY_LIMIT 3
#define WEBSOCKET_SERVER_URL "ws://127.0.0.1:8080"

// IO Types
typedef enum {
    IO_TYPE_TCP,
    IO_TYPE_WEBSOCKET
} io_type_t;

// Unified IO structure
typedef struct {
    io_type_t type;
    int tcp_socket;                 // For TCP/IP
    struct lws *websocket;          // For WebSocket
    struct lws_context *ws_context; // WebSocket context
} unified_io_t;

// IO Configuration structure
typedef struct {
    const char *address;
    int port;
    io_type_t type;
} io_config_t;

// Known configurations (Cascades: API.*, IP.*, Ports.*)
io_config_t known_configs[] = {
    {"127.0.0.1", 8080, IO_TYPE_TCP},
    {"127.0.0.1", 8081, IO_TYPE_WEBSOCKET},
    {"api.example.com", 443, IO_TYPE_WEBSOCKET},
    {"192.168.1.1", 8082, IO_TYPE_TCP},
    {"localhost", 8083, IO_TYPE_WEBSOCKET}
};
int config_count = sizeof(known_configs) / sizeof(io_config_t);

// Logging utility
void log_message(const char *level, const char *message) {
    FILE *log_file = fopen(LOG_FILE, "a");
    if (log_file) {
        time_t now = time(NULL);
        char *timestamp = ctime(&now);
        timestamp[strlen(timestamp) - 1] = '\0'; // Remove newline
        fprintf(log_file, "[%s] [%s] %s\n", level, timestamp, message);
        fclose(log_file);
    } else {
        fprintf(stderr, "Failed to open log file: %s\n", LOG_FILE);
    }
}

// TCP/IP Functions
int init_tcp_socket(const char *ip, int port) {
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        log_message("ERROR", "Failed to create TCP socket");
        return -1;
    }

    struct sockaddr_in server_addr = {0};
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);

    if (inet_pton(AF_INET, ip, &server_addr.sin_addr) <= 0) {
        log_message("ERROR", "Invalid IP address");
        close(sockfd);
        return -1;
    }

    if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        log_message("ERROR", "Failed to connect to TCP server");
        close(sockfd);
        return -1;
    }

    log_message("INFO", "Connected to TCP server");
    return sockfd;
}

// WebSocket Functions
struct lws_context *init_websocket_context() {
    struct lws_context_creation_info info = {0};
    info.port = CONTEXT_PORT_NO_LISTEN;
    info.protocols = (struct lws_protocols[]){
        {"ws-protocol", NULL, 0, BUFFER_SIZE},
        {NULL, NULL, 0, 0}};
    return lws_create_context(&info);
}

struct lws *connect_websocket(struct lws_context *ctx, const char *address, int port) {
    struct lws_client_connect_info connect_info = {0};
    connect_info.context = ctx;
    connect_info.address = address;
    connect_info.port = port;
    connect_info.path = "/";
    connect_info.protocol = "ws-protocol";

    return lws_client_connect_via_info(&connect_info);
}

// Unified IO Functions
unified_io_t *init_unified_io(io_config_t *config) {
    unified_io_t *io = malloc(sizeof(unified_io_t));
    if (!io) {
        log_message("ERROR", "Failed to allocate memory for unified IO");
        return NULL;
    }

    io->type = config->type;

    if (config->type == IO_TYPE_TCP) {
        io->tcp_socket = init_tcp_socket(config->address, config->port);
        if (io->tcp_socket < 0) {
            free(io);
            return NULL;
        }
    } else if (config->type == IO_TYPE_WEBSOCKET) {
        io->ws_context = init_websocket_context();
        if (!io->ws_context) {
            free(io);
            return NULL;
        }

        io->websocket = connect_websocket(io->ws_context, config->address, config->port);
        if (!io->websocket) {
            lws_context_destroy(io->ws_context);
            free(io);
            return NULL;
        }
    }

    return io;
}

void cleanup_unified_io(unified_io_t *io) {
    if (!io) return;

    if (io->type == IO_TYPE_TCP) {
        close(io->tcp_socket);
    } else if (io->type == IO_TYPE_WEBSOCKET) {
        lws_context_destroy(io->ws_context);
    }

    free(io);
}

// PMLL-ARLL-EFLL Logic Loop
unified_io_t *dynamic_io_loop() {
    for (int config_index = 0; config_index < config_count; config_index++) {
        log_message("INFO", "Attempting connection...");
        int retries = 0;

        while (retries < RETRY_LIMIT) {
            unified_io_t *io = init_unified_io(&known_configs[config_index]);
            if (io) {
                log_message("INFO", "Successfully connected via known configuration");
                return io;
            }

            log_message("WARNING", "Connection attempt failed. Retrying...");
            retries++;
        }

        log_message("ERROR", "All retries exhausted for the current configuration");
    }

    log_message("FATAL", "All connection attempts failed");
    return NULL;
}

// Main Function
int main() {
    unified_io_t *io = dynamic_io_loop();
    if (!io) {
        fprintf(stderr, "Failed to establish any IO connection\n");
        return EXIT_FAILURE;
    }

    // Example usage
    const char *message = "Hello, Dynamic IO!";
    if (io->type == IO_TYPE_TCP) {
        send(io->tcp_socket, message, strlen(message), 0);
    } else if (io->type == IO_TYPE_WEBSOCKET) {
        unsigned char buf[LWS_SEND_BUFFER_PRE_PADDING + BUFFER_SIZE + LWS_SEND_BUFFER_POST_PADDING];
        unsigned char *msg = &buf[LWS_SEND_BUFFER_PRE_PADDING];
        size_t len = strlen(message);
        memcpy(msg, message, len);
        lws_write(io->websocket, msg, len, LWS_WRITE_TEXT);
    }

    cleanup_unified_io(io);
    return EXIT_SUCCESS;
}
