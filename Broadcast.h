#ifndef BROADCAST_H
#define BROADCAST_H

#include "4Dblockchain.h"

// Function prototypes
void initializeBroadcast();                        // Initialize broadcasting system
char* createTransactionWithSerializedData(BlockchainBlock* block); // Create a transaction for a specific block
int broadcastTransaction(const char* rawTxHex);   // Broadcast raw transaction to the network
void serializeAndBroadcastUpToHeight(uint32_t targetHeight); // Serialize and broadcast up to a specific height
void cleanupBroadcast();                          // Cleanup broadcasting system

#endif // BROADCAST_H
