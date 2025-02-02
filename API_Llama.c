#include "API.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h> // For close()

// Function to handle PMLL logic
void handle_pmll_logic(int sockfd) {
    const char* request = "GET /pmll/logic HTTP/1.1\r\nHost: localhost:8082\r\n\r\n";
    char response[BUFFER_SIZE];

    printf("Sending PMLL Logic Request...\n");
    send_request(sockfd, request);
    receive_response(sockfd, response);
    printf("PMLL Response: %s\n", response);

    // Add PMLL-specific processing here
    if (strstr(response, "SUCCESS")) {
        printf("PMLL Logic executed successfully.\n");
    } else {
        printf("PMLL Logic failed or returned an unknown state.\n");
    }
}

// Function to handle ARLL logic
void handle_arll_logic(int sockfd) {
    const char* request = "GET /arll/logic HTTP/1.1\r\nHost: localhost:8083\r\n\r\n";
    char response[BUFFER_SIZE];

    printf("Sending ARLL Logic Request...\n");
    send_request(sockfd, request);
    receive_response(sockfd, response);
    printf("ARLL Response: %s\n", response);

    // Add ARLL-specific processing here
    if (strstr(response, "REWARD")) {
        printf("ARLL rewarded successfully.\n");
    } else {
        printf("ARLL Logic failed or returned an unknown state.\n");
    }
}

// Function to handle EFLL logic
void handle_efll_logic(int sockfd) {
    const char* request = "GET /efll/logic HTTP/1.1\r\nHost: localhost:8084\r\n\r\n";
    char response[BUFFER_SIZE];

    printf("Sending EFLL Logic Request...\n");
    send_request(sockfd, request);
    receive_response(sockfd, response);
    printf("EFLL Response: %s\n", response);

    // Add EFLL-specific processing here
    if (strstr(response, "CLEAN")) {
        printf("EFLL verified memory is clean.\n");
    } else if (strstr(response, "CORRUPT")) {
        printf("EFLL detected memory corruption!\n");
    } else {
        printf("EFLL Logic returned an unknown state.\n");
    }
}

// Main function for cascading Llama logic
void run_llama_cascade() {
    int pmll_sockfd, arll_sockfd, efll_sockfd;
    struct sockaddr_in pmll_server_addr, arll_server_addr, efll_server_addr;

    // Create and connect to PMLL server
    pmll_sockfd = create_socket();
    set_up_server_address(&pmll_server_addr, PMLL_URL, PMLL_PORT);
    connect_to_server(pmll_sockfd, &pmll_server_addr);

    // Create and connect to ARLL server
    arll_sockfd = create_socket();
    set_up_server_address(&arll_server_addr, ARLL_URL, ARLL_PORT);
    connect_to_server(arll_sockfd, &arll_server_addr);

    // Create and connect to EFLL server
    efll_sockfd = create_socket();
    set_up_server_address(&efll_server_addr, EFLL_URL, EFLL_PORT);
    connect_to_server(efll_sockfd, &efll_server_addr);

    // Execute logic loops
    printf("\nStarting Llama Logic Cascade...\n");
    handle_pmll_logic(pmll_sockfd);
    handle_arll_logic(arll_sockfd);
    handle_efll_logic(efll_sockfd);

    // Close sockets
    close(pmll_sockfd);
    close(arll_sockfd);
    close(efll_sockfd);

    printf("\nLlama Logic Cascade completed.\n");
}
