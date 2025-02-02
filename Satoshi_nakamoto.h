#ifndef SATOSHI_NAKAMOTO_H
#define SATOSHI_NAKAMOTO_H

// Declare the function to fetch the latest block height
int fetch_block_height(void);

// Declare the function to create and sign the OP_RETURN transaction
void create_broadcast_transaction(const char* declaration_message);

// Declare the function to broadcast the transaction
void broadcast_transaction(void);

// Declaration for looping through blockchain blocks
void run_broadcast_loop(void);

#endif // SATOSHI_NAKAMOTO_H
