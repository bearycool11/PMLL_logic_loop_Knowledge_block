#ifndef GOLDEN_BRAID_H
#define GOLDEN_BRAID_H

// Includes
#include <stdbool.h>

// Constants
#define MAX_OP_RETURN_SIZE 80

// Function Prototypes
char* loadPrivateKey(const char* filepath);
bool validatePrivateKey(const char* privateKey, const char* walletAddress);
void createTransactionWithOPReturn(const char* message);
void broadcastWithApiKey(const char* transaction);

#endif // GOLDEN_BRAID_H
