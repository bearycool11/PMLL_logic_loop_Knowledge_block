#include <iostream>
#include <string>
#include <vector>
#include <openssl/sha.h>
#include <openssl/ripemd.h>
#include <openssl/ec.h>
#include <openssl/obj_mac.h>

// Constants
#define DEFAULT_FEE 0.0001  // Default fee in BTC

// Utility functions for hashing
std::string sha256(const std::string &input) {
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256((unsigned char *)input.c_str(), input.size(), hash);

    char outputBuffer[65];
    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
        sprintf(outputBuffer + (i * 2), "%02x", hash[i]);
    }
    outputBuffer[64] = 0;
    return std::string(outputBuffer);
}

std::string ripemd160(const std::string &input) {
    unsigned char hash[RIPEMD160_DIGEST_LENGTH];
    RIPEMD160((unsigned char *)input.c_str(), input.size(), hash);

    char outputBuffer[41];
    for (int i = 0; i < RIPEMD160_DIGEST_LENGTH; i++) {
        sprintf(outputBuffer + (i * 2), "%02x", hash[i]);
    }
    outputBuffer[40] = 0;
    return std::string(outputBuffer);
}

// Bitcoin transaction structure
struct BitcoinTransaction {
    std::string txid;
    std::string sender_address;
    std::string recipient_address;
    double amount;
    double fee;
    bool signed_transaction = false;

    void broadcast() {
        if (signed_transaction) {
            std::cout << "Transaction broadcasted successfully!" << std::endl;
        } else {
            std::cout << "Transaction signing failed. Cannot broadcast." << std::endl;
        }
    }
};

// Bitcoin Wallet class
class Wallet {
public:
    Wallet(const std::string &name) : wallet_name(name) {
        generate_keys();
        generate_address();
    }

    std::string get_address() const {
        return address;
    }

    BitcoinTransaction create_transaction(const std::string &recipient, double amount, double fee = DEFAULT_FEE) {
        if (amount + fee > balance) {
            throw std::runtime_error("Insufficient balance for the transaction.");
        }

        BitcoinTransaction transaction;
        transaction.txid = sha256(recipient + std::to_string(amount) + std::to_string(fee));
        transaction.sender_address = address;
        transaction.recipient_address = recipient;
        transaction.amount = amount;
        transaction.fee = fee;
        transaction.signed_transaction = sign_transaction(transaction);

        return transaction;
    }

    void add_balance(double amount) {
        balance += amount;
    }

private:
    std::string wallet_name;
    std::string private_key;
    std::string public_key;
    std::string address;
    double balance = 0.0;

    void generate_keys() {
        // Simplified key generation using OpenSSL
        EC_KEY *key = EC_KEY_new_by_curve_name(NID_secp256k1);
        EC_KEY_generate_key(key);

        const BIGNUM *priv_bn = EC_KEY_get0_private_key(key);
        char *priv_hex = BN_bn2hex(priv_bn);
        private_key = std::string(priv_hex);

        EC_KEY_free(key);
        OPENSSL_free(priv_hex);
    }

    void generate_address() {
        // Simplified address generation
        std::string hash1 = sha256(public_key);
        std::string hash2 = ripemd160(hash1);
        address = hash2;  // This would be further encoded in real implementations
    }

    bool sign_transaction(const BitcoinTransaction &transaction) {
        // Placeholder for real signing logic
        std::cout << "Signing transaction for recipient: " << transaction.recipient_address << std::endl;
        return true;
    }
};

// Example usage
int main() {
    try {
        Wallet wallet("MyWallet");
        wallet.add_balance(0.01);

        std::cout << "Wallet Address: " << wallet.get_address() << std::endl;

        BitcoinTransaction transaction = wallet.create_transaction("1A1zP1eP5QGefi2DMPTfTL5SLmv7DivfNa", 0.001);
        std::cout << "Transaction ID: " << transaction.txid << std::endl;

        transaction.broadcast();
    } catch (const std::exception &e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }

    return 0;
}
