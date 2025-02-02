#ifndef MINER_H
#define MINER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Constants
#define TARGET_PREFIX "0000"   // Difficulty target (hash must start with this)
#define TEAM_WALLET_ADDRESS "bc1qetkudft7hlsl3k7nhrg6zrkufpu6q3rdnx5ag5" // Wallet address

// Structures
typedef struct {
    char wallet_address[64];     // Wallet address
    unsigned char bitcoin[32];   // Bitcoin transaction data
} bitcoin_transaction_t;

// Function Prototypes

/*
