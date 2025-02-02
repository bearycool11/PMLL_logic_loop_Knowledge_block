#ifndef FOUR_D_BLOCKCHAIN_H
#define FOUR_D_BLOCKCHAIN_H

#include <stdint.h>

// Blockchain block structure
typedef struct {
    uint32_t height;        // Block height
    char hash[65];          // Block hash (64 characters + null terminator)
    uint32_t timestamp;     // Block timestamp
    double reward;          // Block reward
    int transactionCount;   // Number of transactions
    char immutableMessage[256]; // Immutable message tied to block
    char schema[256];       // Schema representation (visual node and linkage)
} BlockchainBlock;

// Blockchain lattice node structure
typedef struct {
    int x, y, z, t;         // 4D coordinates
    BlockchainBlock* block; // Pointer to blockchain block data
} BlockchainLatticeNode;

// Function prototypes
void initializeGenesisBlock();  // Initialize genesis block with NULL coin
void serializeBlockchainUpToHeight(uint32_t targetHeight); // Serialize blockchain
void embedSchemaRepresentation(BlockchainBlock* block); // Add schema representation
void storeSerializedDataInLattice(uint32_t height, BlockchainBlock* block); // Store serialized data
void incrementBlock(uint32_t currentHeight);  // Increment block height
void cleanup4DBlockchain();  // Free allocated resources
void printBlockInfo(BlockchainBlock* block);  // Print block details

#endif // FOUR_D_BLOCKCHAIN_H
