#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>

// Define 4D matrix structure
typedef struct {
    int dim1, dim2, dim3, dim4;
    float *data; // Flattened 4D data
} Matrix4D;

// Function to create a 4D matrix
Matrix4D* create_matrix4d(int d1, int d2, int d3, int d4) {
    Matrix4D* mat = (Matrix4D*)malloc(sizeof(Matrix4D));
    mat->dim1 = d1;
    mat->dim2 = d2;
    mat->dim3 = d3;
    mat->dim4 = d4;
    mat->data = (float*)malloc(d1 * d2 * d3 * d4 * sizeof(float));
    return mat;
}

// Networking setup
void* broadcast_matrix(void* arg) {
    Matrix4D* matrix = (Matrix4D*)arg;
    int server_fd;
    struct sockaddr_in broadcast_addr;
    int broadcast_enable = 1;

    if ((server_fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    if (setsockopt(server_fd, SOL_SOCKET, SO_BROADCAST, &broadcast_enable, sizeof(broadcast_enable)) < 0) {
        perror("Failed to enable broadcast");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    memset(&broadcast_addr, 0, sizeof(broadcast_addr));
    broadcast_addr.sin_family = AF_INET;
    broadcast_addr.sin_port = htons(8080);
    broadcast_addr.sin_addr.s_addr = inet_addr("255.255.255.255");

    // Send 4D matrix data
    if (sendto(server_fd, matrix->data, matrix->dim1 * matrix->dim2 * matrix->dim3 * matrix->dim4 * sizeof(float),
               0, (struct sockaddr*)&broadcast_addr, sizeof(broadcast_addr)) < 0) {
        perror("Broadcast failed");
    }

    close(server_fd);
    return NULL;
}

int main() {
    Matrix4D* matrix = create_matrix4d(4, 4, 4, 4); // Example: 4x4x4x4 matrix
    for (int i = 0; i < 256; ++i) {
        matrix->data[i] = (float)i; // Fill matrix with sample data
    }

    pthread_t broadcast_thread;
    if (pthread_create(&broadcast_thread, NULL, broadcast_matrix, (void*)matrix) != 0) {
        perror("Thread creation failed");
        return EXIT_FAILURE;
    }

    pthread_join(broadcast_thread, NULL);
    free(matrix->data);
    free(matrix);

    return 0;
}
