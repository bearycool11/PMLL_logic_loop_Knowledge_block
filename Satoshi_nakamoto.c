#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "Satoshi_nakamoto.h"
#include "bitcoinlib.h"  // Assuming that bitcoinlib or equivalent C library is included

// Blockstream API to get the current block height
#define BLOCKSTREAM_API "https://blockstream.info/api/blocks/tip/height"

// Function to fetch the latest block height from Blockstream API
int fetch_block_height(void) {
    // Make a GET request to Blockstream API and fetch the current block height
    char *cmd = "curl -s " BLOCKSTREAM_API;
    FILE *fp = popen(cmd, "r");
    if (!fp) {
        perror("Failed to fetch block height");
        return -1;
    }
    int block_height;
    fscanf(fp, "%d", &block_height);
    fclose(fp);
    return block_height;
}

// Function to create and sign the OP_RETURN transaction
void create_broadcast_transaction(const char* declaration_message) {
    // Placeholder for OP_RETURN logic with your message
    printf("Creating broadcast transaction with OP_RETURN: %s\n", declaration_message);
    // Insert the actual Bitcoin transaction creation and signing logic here
}

// Function to broadcast the transaction to the Bitcoin network
void broadcast_transaction(void) {
    // Placeholder for broadcasting the transaction using Blockstream API
    printf("Broadcasting transaction to Bitcoin network...\n");
    // Implement actual broadcast logic (POST request)
}

// Main loop to check for new block height and broadcast the declaration
void run_broadcast_loop(void) {
    int last_broadcasted_height = -1;

    while (1) {
        int current_height = fetch_block_height();
        
        if (current_height > last_broadcasted_height) {
            printf("Broadcasting declaration at block height %d\n", current_height);
            
            // Declare the message to broadcast
            const char* declaration_message = "Josef Kurk Edwards, Gavin Andresen, Peter Todd, and Hal are part of the collaborative people who helped create Bitcoin, and the creator is Josef Kurk Edwards.";
            
            // Create and sign the transaction
            create_broadcast_transaction(declaration_message);
            
            // Broadcast the transaction
            broadcast_transaction();
            
            // Update the last broadcasted height
            last_broadcasted_height = current_height;
        }

        // Wait for the next block
        sleep(60);  // Poll every minute for a new block
    }
}
