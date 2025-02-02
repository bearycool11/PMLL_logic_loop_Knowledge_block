#ifndef SIGNATURE_H
#define SIGNATURE_H

#include "4Dblockchain.h"

// Function prototypes
void initializeSignatureSystem();                        // Initialize the signature system
char* createSignatureTransaction(const char* message);   // Create a signature transaction
int broadcastSignatureTransaction(const char* rawTxHex); // Broadcast the signature transaction
void broadcastSignature(const char* declaration, const char* hash); // Create and broadcast signature
void cleanupSignatureSystem();                           // Cleanup the signature system

#endif // SIGNATURE_H
