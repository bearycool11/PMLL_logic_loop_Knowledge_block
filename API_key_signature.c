#include "api_key_signature.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include <unistd.h> // For sleep()

// Global variable to store the API key
static char apiKey[API_KEY_MAX_LENGTH];

// Retry limits
#define RETRY_LIMIT 3

// Backup API configurations for failover (ARLL and EFLL logic)
typedef struct {
    const char *apiUrl;
    const char *description;
} api_config_t;

api_config_t backupConfigs[] = {
    {"https://api.backup1.com/v1/broadcast", "Backup API 1"},
    {"https://api.backup2.com/v1/broadcast", "Backup API 2"},
    {"https://api.backup3.com/v1/broadcast", "Backup API 3"}
};
int backupConfigCount = sizeof(backupConfigs) / sizeof(api_config_t);

// Initialize the API key system
void initializeApiKeySignature(const char* key) {
    if (strlen(key) >= API_KEY_MAX_LENGTH) {
        fprintf(stderr, "Error: API key exceeds maximum length.\n");
        exit(EXIT_FAILURE);
    }
    strcpy(apiKey, key);
    printf("API key initialized.\n");
}

// Validate the API key
bool isApiKeyValid() {
    return strlen(apiKey) > 0;
}

// Retry logic with PMLL-ARLL-EFLL
int broadcastTransactionWithRetries(const char* rawTxHex, const char* apiUrl) {
    int retries = 0;
    while (retries < RETRY_LIMIT) {
        printf("Attempting to broadcast transaction via %s (Attempt %d/%d)\n", apiUrl, retries + 1, RETRY_LIMIT);

        CURL *curl = curl_easy_init();
        if (curl) {
            char postData[1024];
            snprintf(postData, sizeof(postData), "{\"tx\": \"%s\"}", rawTxHex);

            struct curl_slist *headers = NULL;
            headers = curl_slist_append(headers, "Content-Type: application/json");
            char authHeader[256];
            snprintf(authHeader, sizeof(authHeader), "X-API-Token: %s", apiKey);
            headers = curl_slist_append(headers, authHeader);

            curl_easy_setopt(curl, CURLOPT_URL, apiUrl);
            curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
            curl_easy_setopt(curl, CURLOPT_POSTFIELDS, postData);

            CURLcode res = curl_easy_perform(curl);

            if (res == CURLE_OK) {
                printf("Transaction broadcast successfully via %s\n", apiUrl);
                curl_easy_cleanup(curl);
                curl_slist_free_all(headers);
                return 0; // Success
            } else {
                fprintf(stderr, "Error: Failed to broadcast via %s: %s\n", apiUrl, curl_easy_strerror(res));
            }

            curl_easy_cleanup(curl);
            curl_slist_free_all(headers);
        } else {
            fprintf(stderr, "Error: CURL initialization failed.\n");
        }

        retries++;
        sleep(1); // Cooldown period
    }

    printf("All retries exhausted for %s\n", apiUrl);
    return -1; // Failure
}

// PMLL-ARLL-EFLL logic to dynamically switch API paths
void broadcastWithApiKey(const char* rawTxHex) {
    if (!isApiKeyValid()) {
        fprintf(stderr, "Error: API key is not initialized or invalid.\n");
        return;
    }

    // Primary API attempt
    printf("Starting broadcast using primary API: %s\n", API_URL);
    if (broadcastTransactionWithRetries(rawTxHex, API_URL) == 0) {
        return; // Success
    }

    // ARLL (Alternate Retry Logic Loop) with backup configurations
    for (int i = 0; i < backupConfigCount; i++) {
        printf("Switching to %s: %s\n", backupConfigs[i].description, backupConfigs[i].apiUrl);
        if (broadcastTransactionWithRetries(rawTxHex, backupConfigs[i].apiUrl) == 0) {
            return; // Success
        }
    }

    // EFLL (Error Flagging Logic Loop) if all backups fail
    fprintf(stderr, "FATAL ERROR: Transaction broadcast failed across all configured endpoints.\n");
}

// Cleanup API key system
void cleanupApiKeySignature() {
    memset(apiKey, 0, sizeof(apiKey)); // Clear the API key
    printf("API key system cleaned up.\n");
}
