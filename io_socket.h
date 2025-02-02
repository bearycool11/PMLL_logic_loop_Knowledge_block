#ifndef UNIFIED_IO_H
#define UNIFIED_IO_H

#include <libwebsockets.h>
#include <stddef.h> // For size_t

// Buffer and Retry Constants
#define BUFFER_SIZE 2048
#define LOG_FILE "io_socket.log"
#define RETRY_LIMIT 5

// IO Types
typedef enum {
    IO_TYPE_TCP,
    IO_TYPE_WEBSOCKET
} io_type_t;

// Unified IO Structure
typedef struct {
    io_type_t type;                // IO type (TCP/WebSocket)
    int tcp_socket;                // TCP socket descriptor
    struct lws *websocket;         // WebSocket handle
    struct lws_context *ws_context; // WebSocket context
} unified_io_t;

// IO Configuration Structure
typedef struct {
    const char *address; // Address or hostname
    int port;            // Port number
    io_type_t type;      // IO type
} io_config_t;

// Known Configurations Array
extern io_config_t known_configs[];
extern int config_count;

// Logging Utility
void log_message(const char *level, const char *message);

// TCP/IP Functions
int init_tcp_socket(const char *ip, int port);
int tcp_send(int socket, const char *message);
int tcp_receive(int socket, char *buffer, size_t buffer_size);

// WebSocket Functions
struct lws_context *init_websocket_context();
struct lws *connect_websocket(struct lws_context *ctx, const char *address, int port);
void websocket_send(struct lws *wsi, const char *message);

// Unified IO Functions
unified_io_t *init_unified_io(io_config_t *config);
void send_message(unified_io_t *io, const char *message);
void receive_message(unified_io_t *io, char *buffer, size_t buffer_size);
void cleanup_unified_io(unified_io_t *io);

// Dynamic Logic Loop
unified_io_t *dynamic_io_loop();

#endif // UNIFIED_IO_H
