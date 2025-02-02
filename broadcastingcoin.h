#ifndef BROADCASTINGCOIN_H
#define BROADCASTINGCOIN_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/sha.h>
#include <openssl/aes.h>

// Bitcoin transaction structure
typedef struct {
    char wallet_address[64];     // Wallet address
    unsigned char bitcoin[32];   // Bitcoin transaction data
} bitcoin_transaction_t;

// Dependency and permissions management
void install_dependencies();                        // Installs required dependencies
void set_execute_permissions();                    // Sets executable permissions for scripts
void execute_secure_command(const char *command);  // Executes secure system commands

// Bitcoin transaction handling
bitcoin_transaction_t *create_bitcoin_transaction(const char *wallet_address, unsigned char *bitcoin);
void sign_bitcoin_transaction(bitcoin_transaction_t *transaction);
void broadcast_bitcoin_transaction(bitcoin_transaction_t *transaction);
void send_bitcoin_to_wallet(const char *wallet_address, unsigned char *bitcoin);

// Advanced Bitcoin operations
void sign_bitcoin_transaction_with_key(char* tx_hex, const char* private_key); // Signs a Bitcoin transaction with a private key
int send_bitcoin_transaction(const char* raw_tx);                              // Sends a signed Bitcoin transaction to the network
void process_bitcoin_payment(const char* recipient, double amount);            // Processes Bitcoin payments
void create_and_sign_transaction(const char* recipient_address, double amount);// Creates and signs a Bitcoin transaction

// PMLL Utilities (Hashing, Compression, and Caching)
void PMLL_Hash_Function(const char *input, unsigned char *output); // Double-SHA-256 hash
void PMLL_Compress_Data(const char *input, unsigned char *output); // AES and SHA-256 compression
void PMLL_Cache_Data(const char *input, unsigned char *output);    // Simple caching mechanism

#endif // BROADCASTINGCOIN_H


#ifndef BROADCASTINGCOIN_H
#define BROADCASTINGCOIN_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/sha.h>
#include <openssl/aes.h>

// Bitcoin transaction structure
typedef struct {
    char wallet_address[64];     // Wallet address
    unsigned char bitcoin[32];   // Bitcoin transaction data
} bitcoin_transaction_t;

// Dependency and permissions management
void install_dependencies();                        // Installs required dependencies
void set_execute_permissions();                    // Sets executable permissions for scripts
void execute_secure_command(const char *command);  // Executes secure system commands

// Bitcoin transaction handling
bitcoin_transaction_t *create_bitcoin_transaction(const char *wallet_address, unsigned char *bitcoin);
void sign_bitcoin_transaction(bitcoin_transaction_t *transaction);
void broadcast_bitcoin_transaction(bitcoin_transaction_t *transaction);
void send_bitcoin_to_wallet(const char *wallet_address, unsigned char *bitcoin);

// Advanced Bitcoin operations
void sign_bitcoin_transaction_with_key(char* tx_hex, const char* private_key); // Signs a Bitcoin transaction with a private key
int send_bitcoin_transaction(const char* raw_tx);                              // Sends a signed Bitcoin transaction to the network
void process_bitcoin_payment(const char* recipient, double amount);            // Processes Bitcoin payments
void create_and_sign_transaction(const char* recipient_address, double amount);// Creates and signs a Bitcoin transaction

// PMLL Utilities (Hashing, Compression, and Caching)
void PMLL_Hash_Function(const char *input, unsigned char *output); // Double-SHA-256 hash
void PMLL_Compress_Data(const char *input, unsigned char *output); // AES and SHA-256 compression
void PMLL_Cache_Data(const char *input, unsigned char *output);    // Simple caching mechanism

#endif // BROADCASTINGCOIN_H


#ifndef COIN_H
#define COIN_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Define the Bitcoin transaction structure
typedef struct {
    const char *wallet_address;
    unsigned char *bitcoin;
} bitcoin_transaction_t;

// Function declarations for Bitcoin transaction handling
void install_dependencies();  // Installs required dependencies for Bitcoin operations
void execute_secure_command(const char *command);  // Executes system commands with sudo
void send_bitcoin_to_wallet(const char *wallet_address, unsigned char *bitcoin);  // Sends Bitcoin to the specified wallet
void PMLL_Hash_Function(const char *input, unsigned char *output);  // Hashes input data
void PMLL_Compress_Data(const char *input, unsigned char *output);  // Compresses data with AES and SHA-256
void PMLL_Cache_Data(const char *input, unsigned char *output);  // Caches data for later use

// Function for Bitcoin transaction signing
void sign_bitcoin_transaction(char* tx_hex, const char* private_key);

// Function to send signed Bitcoin transaction to the Bitcoin network
int send_bitcoin_transaction(const char* raw_tx);

// Function to process Bitcoin payments
void process_bitcoin_payment(const char* recipient, double amount);

// Function to create and sign a Bitcoin transaction
void create_and_sign_transaction(const char* recipient_address, double amount);

#endif // COIN_H

#ifndef BROADCASTINGCOIN_H
#define BROADCASTINGCOIN_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/sha.h>
#include <openssl/aes.h>

// Define the Bitcoin transaction structure
typedef struct {
    char wallet_address[64];
    unsigned char bitcoin[32];
} bitcoin_transaction_t;

// Function declarations
void install_dependencies(); // Installs dependencies required for Bitcoin operations
void set_execute_permissions(); // Sets executable permissions for necessary scripts
void execute_secure_command(const char *command); // Executes secure system commands

// Bitcoin transaction handling
bitcoin_transaction_t *create_bitcoin_transaction(const char *wallet_address, unsigned char *bitcoin);
void sign_bitcoin_transaction(bitcoin_transaction_t *transaction);
void broadcast_bitcoin_transaction(bitcoin_transaction_t *transaction);
void send_bitcoin_to_wallet(const char *wallet_address, unsigned char *bitcoin);

// PMLL (Hashing, Compression, and Caching) Utilities
void PMLL_Hash_Function(const char *input, unsigned char *output);
void PMLL_Compress_Data(const char *input, unsigned char *output);
void PMLL_Cache_Data(const char *input, unsigned char *output);

#endif // BROADCASTINGCOIN_H

