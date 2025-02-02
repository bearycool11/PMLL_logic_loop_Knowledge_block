#ifndef BROADCAST_H
#define BROADCAST_H

#include "4Dblockchain.h"

// Function prototypes
void initializeBroadcast();                        // Initialize broadcasting system
char* createTransactionWithOPReturn(const char* message);  // Create a transaction with OP_RETURN
int broadcastTransaction(const char* rawTxHex);   // Broadcast raw transaction to the network
void cleanupBroadcast();                          // Cleanup broadcasting system

#endif // BROADCAST_H
