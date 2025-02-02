#ifndef API_KEY_SIGNATURE_H
#define API_KEY_SIGNATURE_H

#include <stdbool.h>

// Maximum length for the API key
#define API_KEY_MAX_LENGTH 256

// Primary API URL
#define API_URL "https://api.primary.com/v1/broadcast"

// Retry limits
#define RETRY_LIMIT 3

// Function Declarations

/**
 * Initializes the API key system with the provided API key.
 * @param key The API key to initialize.
 */
void initializeApiKeySignature(const char* key);

/**
 * Validates if the API key is properly initialized and valid.
 * @return True if the API key is valid, false otherwise.
 */
bool isApiKeyValid();

/**
 * Broadcasts a transaction using the configured API key.
 * Implements PMLL (Primary Retry), ARLL (Alternate Retry), and EFLL (Error Flagging).
 * @param rawTxHex The raw transaction hex to broadcast.
 */
void broadcastWithApiKey(const char* rawTxHex);

/**
 * Cleans up the API key system by clearing the stored API key.
 */
void cleanupApiKeySignature();

#endif // API_KEY_SIGNATURE_H
