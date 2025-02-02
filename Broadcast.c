#include "broadcast.h"
#include "4Dblockchain.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Initialize Broadcasting System
void initializeBroadcast() {
    printf("Broadcasting system initialized.\n");
}

// Create a Transaction with OP_RETURN
char* createTransactionWithOPReturn(const char* message) {
    if (strlen(message) > 80) {
        printf("Error: OP_RETURN messages must be 80 bytes or less.\n");
        return NULL;
    }

    // Example: Create a raw transaction with a dummy UTXO
    char* rawTx = (char*)malloc(512); // Allocate space for raw transaction
    sprintf(rawTx, "0100000001abcdefabcdefabcdefabcdefabcdefabcdefabcdefabcdef0000000000ffffffff01%02x%s00000000",
            (int)strlen(message),
            message);

    printf("Transaction Created: %s\n", rawTx);
    return rawTx;
}

// Create a Transaction for a Serialized Block
char* createTransactionWithSerializedData(BlockchainBlock* block) {
    if (!block) {
        printf("Error: Block data is null.\n");
        return NULL;
    }

    // Ensure the serialized data fits within the OP_RETURN limit
    if (strlen(block->immutableMessage) > 80) {
        printf("Error: Serialized block message exceeds 80 bytes.\n");
        return NULL;
    }

    // Example: Create a raw transaction with a dummy UTXO
    char* rawTx = (char*)malloc(512); // Allocate space for raw transaction
    sprintf(rawTx, "0100000001abcdefabcdefabcdefabcdefabcdefabcdefabcdefabcdef0000000000ffffffff01%02x%s00000000",
            (int)strlen(block->immutableMessage),
            block->immutableMessage);

    printf("Transaction Created for Block %u: %s\n", block->height, rawTx);
    return rawTx;
}

// Broadcast Raw Transaction
int broadcastTransaction(const char* rawTxHex) {
    if (!rawTxHex) {
        printf("Error: Cannot broadcast a null transaction.\n");
        return -1;
    }

    // Simulate broadcast
    printf("Broadcasting Transaction: %s\n", rawTxHex);

    // Simulate successful broadcast
    printf("Transaction Broadcast Successfully!\n");
    return 0;
}

// Serialize and Broadcast Up to a Specific Height
void serializeAndBroadcastUpToHeight(uint32_t targetHeight) {
    initializeGenesisBlock();
    BlockchainBlock* currentBlock = genesisBlock;

    for (uint32_t height = 1; height <= targetHeight; height++) {
        BlockchainBlock* newBlock = incrementBlock(currentBlock->height);
        storeSerializedDataInLattice(height, newBlock);

        // Create and broadcast transaction for the serialized block
        char* rawTx = createTransactionWithSerializedData(newBlock);
        if (rawTx) {
            broadcastTransaction(rawTx);
            free(rawTx); // Free allocated memory
        }

        currentBlock = newBlock;
    }

    printf("Broadcasting Complete Up to Height: %u\n", targetHeight);
}

// Cleanup Broadcasting System
void cleanupBroadcast() {
    cleanup4DBlockchain();
    printf("Broadcasting system cleaned up.\n");
}
