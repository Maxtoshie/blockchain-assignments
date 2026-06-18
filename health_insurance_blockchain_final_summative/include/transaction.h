#ifndef TRANSACTION_H
#define TRANSACTION_H

#include <stdint.h>
#include <time.h>
#include <stdbool.h>

#define MAX_ADDRESS_LENGTH 64
#define MAX_SIGNATURE_LENGTH 128
#define MAX_TRANSACTION_TYPE 32
#define SHA256_DIGEST_LENGTH 32

/**
 * enum transaction_type - Types of transactions
 */
typedef enum {
    TX_POLICY_ENROLLMENT,
    TX_PREMIUM_PAYMENT,
    TX_REINSURANCE_CONTRIBUTION,
    TX_SERVICE_REQUEST,
    TX_PREAUTHORIZATION,
    TX_CLAIM_SUBMISSION,
    TX_CLAIM_APPROVAL,
    TX_CLAIM_REJECTION,
    TX_CLAIM_SETTLEMENT,
    TX_TOKEN_TRANSFER,
    TX_MINING_REWARD,
    TX_POOL_REWARD
} transaction_type_t;

/**
 * struct transaction - Represents a single transaction
 */
typedef struct transaction {
    char transaction_id[64];
    char sender_address[MAX_ADDRESS_LENGTH];
    char receiver_address[MAX_ADDRESS_LENGTH];
    double amount;
    transaction_type_t type;
    time_t timestamp;
    uint64_t sender_nonce;
    unsigned char digital_signature[MAX_SIGNATURE_LENGTH];
    char metadata[256];
    int signature_len;
} transaction_t;

/* Function prototypes */
transaction_t *create_transaction(const char *sender, const char *receiver,
                                  double amount, transaction_type_t type);
void free_transaction(transaction_t *tx);
int validate_transaction(const transaction_t *tx);
void compute_transaction_hash(const transaction_t *tx, unsigned char *hash);
char *transaction_type_to_string(transaction_type_t type);
int verify_transaction_signature(const transaction_t *tx);
void sign_transaction(transaction_t *tx, const unsigned char *private_key);
int find_transaction_in_chain(const char *tx_id);
int find_transaction_in_mempool(const char *tx_id);

#endif /* TRANSACTION_H */
