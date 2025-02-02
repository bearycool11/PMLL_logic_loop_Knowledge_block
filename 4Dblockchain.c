#include "4Dblockchain.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Global 4D lattice array
BlockchainLatticeNode* blockchainLattice[10][10][10][10];

// Genesis block data
BlockchainBlock* genesisBlock;

// Initialize the Genesis Block
void initializeGenesisBlock() {
    genesisBlock = (BlockchainBlock*)malloc(sizeof(BlockchainBlock));
    genesisBlock->height = 0;
    strcpy(genesisBlock->hash, "0000000000000000000000000000000000000000000000000000000000000000");
    genesisBlock->timestamp = 1231006505;  // January 3, 2009
    genesisBlock->reward = 50.0;          // Initial reward
    genesisBlock->transactionCount = 1;  // Genesis transaction

    embedSchemaRepresentation(genesisBlock); // Embed schema in genesis block

    printf("Genesis Block Initialized:\n");
    printBlockInfo(genesisBlock);
}

// Embed Schema Representation
void embedSchemaRepresentation(BlockchainBlock* block) {
    sprintf(block->schema, 
            "Node | Height: %u | Hash: %s | Immutable Declaration: %s",
            block->height, 
            block->hash,
            "4a6f736566204b75726b2045647761726473206973205361746f736869204e616b616d6f746f2e204368616e63656c6c6f72206f6e206272696e6b206f66207365636f6e64206261696c6f757420666f722062616e6b73202d2030332f4a616e2f323030392e");
    strcpy(block->immutableMessage, block->schema); // Store schema as the immutable message
}

// Increment a Block
BlockchainBlock* incrementBlock(uint32_t currentHeight) {
    BlockchainBlock* newBlock = (BlockchainBlock*)malloc(sizeof(BlockchainBlock));
    newBlock->height = currentHeight + 1;
    sprintf(newBlock->hash, 
            "0000000000000000%xabcdef1234567890abcdef1234567890abcdef", 
            newBlock->height);
    newBlock->timestamp = 1231006505 + newBlock->height * 600;
    newBlock->reward = newBlock->height < 210000 ? 50.0 : 25.0;
    newBlock->transactionCount = 10 + (newBlock->height % 5);

    embedSchemaRepresentation(newBlock); // Embed schema in the block

    return newBlock;
}

// Store Serialized Data in the Lattice
void storeSerializedDataInLattice(uint32_t height, BlockchainBlock* block) {
    int x = (height / 1000) % 10;
    int y = (height / 100) % 10;
    int z = (height / 10) % 10;
    int t = height % 10;

    if (!blockchainLattice[x][y][z][t]) {
        blockchainLattice[x][y][z][t] = (BlockchainLatticeNode*)malloc(sizeof(BlockchainLatticeNode));
    }

    blockchainLattice[x][y][z][t]->x = x;
    blockchainLattice[x][y][z][t]->y = y;
    blockchainLattice[x][y][z][t]->z = z;
    blockchainLattice[x][y][z][t]->t = t;
    blockchainLattice[x][y][z][t]->block = block;

    printf("Stored Block %u at Lattice Node (%d, %d, %d, %d):\n", height, x, y, z, t);
    printf("%s\n", block->schema);
}

// Serialize the Blockchain Up to a Target Height
void serializeBlockchainUpToHeight(uint32_t targetHeight) {
    initializeGenesisBlock();
    BlockchainBlock* currentBlock = genesisBlock;

    for (uint32_t height = 1; height <= targetHeight; height++) {
        BlockchainBlock* newBlock = incrementBlock(currentBlock->height);
        storeSerializedDataInLattice(height, newBlock);
        currentBlock = newBlock;
    }

    printf("Serialization Complete Up to Height: %u\n", targetHeight);
}

// Print Block Information
void printBlockInfo(BlockchainBlock* block) {
    printf("Block Height: %u\n", block->height);
    printf("Hash: %s\n", block->hash);
    printf("Timestamp: %u\n", block->timestamp);
    printf("Reward: %.2f BTC\n", block->reward);
    printf("Transactions: %d\n", block->transactionCount);
    printf("Schema: %s\n", block->schema);
}

// Cleanup Function
void cleanup4DBlockchain() {
    for (int x = 0; x < 10; x++) {
        for (int y = 0; y < 10; y++) {
            for (int z = 0; z < 10; z++) {
                for (int t = 0; t < 10; t++) {
                    if (blockchainLattice[x][y][z][t]) {
                        free(blockchainLattice[x][y][z][t]->block);
                        free(blockchainLattice[x][y][z][t]);
                    }
                }
            }
        }
    }
    free(genesisBlock);
    printf("4D Blockchain Lattice Cleanup Complete.\n");
}
