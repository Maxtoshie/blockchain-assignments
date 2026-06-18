#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "transaction.h"
#include "cryptography.h"
#include "utils.h"

/**
 * create_transaction - Create a new transaction
 */
transaction_t *create_transaction(const char *sender, const char *receiver,
                                  double amount, transaction_type_t type)
{
    transaction_t *tx;
    
    tx = (transaction_t *)calloc(1, sizeof(transaction_t));
    if (!tx)
        return NULL;
    
    /* Generate transaction ID */
    char *id = generate_id("TX");
    if (id) {
        strncpy(tx->transaction_id, id, sizeof(tx->transaction_id) - 1);
        free(id);
    }
    
    strncpy(tx->sender_address, sender, MAX_ADDRESS_LENGTH - 1);
    strncpy(tx->receiver_address, receiver, MAX_ADDRESS_LENGTH - 1);
    tx->amount = amount;
    tx->type = type;
    tx->timestamp = time(NULL);
    tx->sender_nonce = 0;
    tx->signature_len = 0;
    memset(tx->digital_signature, 0, MAX_SIGNATURE_LENGTH);
    memset(tx->metadata, 0, sizeof(tx->metadata));
    
    return tx;
}

/**
 * free_transaction - Free a transaction
 */
void free_transaction(transaction_t *tx)
{
    if (tx)
        free(tx);
}

/**
 * validate_transaction - Validate a transaction
 */
int validate_transaction(const transaction_t *tx)
{
    if (!tx)
        return -1;
    
    if (tx->amount <= 0)
        return -1;
    
    if (strlen(tx->sender_address) == 0 || strlen(tx->receiver_address) == 0)
        return -1;
    
    return 0;
}

/**
 * compute_transaction_hash - Compute hash of transaction fields
 */
void compute_transaction_hash(const transaction_t *tx, unsigned char *hash)
{
    unsigned char buffer[1024];
    size_t offset = 0;
    
    if (!tx || !hash)
        return;
    
    /* Serialize transaction fields (excluding signature and hash) */
    memcpy(buffer + offset, tx->transaction_id, strlen(tx->transaction_id));
    offset += strlen(tx->transaction_id);
    memcpy(buffer + offset, tx->sender_address, strlen(tx->sender_address));
    offset += strlen(tx->sender_address);
    memcpy(buffer + offset, tx->receiver_address, strlen(tx->receiver_address));
    offset += strlen(tx->receiver_address);
    memcpy(buffer + offset, &tx->amount, sizeof(tx->amount));
    offset += sizeof(tx->amount);
    memcpy(buffer + offset, &tx->type, sizeof(tx->type));
    offset += sizeof(tx->type);
    memcpy(buffer + offset, &tx->timestamp, sizeof(tx->timestamp));
    offset += sizeof(tx->timestamp);
    memcpy(buffer + offset, &tx->sender_nonce, sizeof(tx->sender_nonce));
    offset += sizeof(tx->sender_nonce);
    memcpy(buffer + offset, tx->metadata, strlen(tx->metadata));
    offset += strlen(tx->metadata);
    
    sha256_hash(buffer, offset, hash);
}

/**
 * transaction_type_to_string - Convert transaction type to string
 */
char *transaction_type_to_string(transaction_type_t type)
{
    switch (type) {
        case TX_POLICY_ENROLLMENT: return "POLICY_ENROLLMENT";
        case TX_PREMIUM_PAYMENT: return "PREMIUM_PAYMENT";
        case TX_REINSURANCE_CONTRIBUTION: return "REINSURANCE_CONTRIBUTION";
        case TX_SERVICE_REQUEST: return "SERVICE_REQUEST";
        case TX_PREAUTHORIZATION: return "PREAUTHORIZATION";
        case TX_CLAIM_SUBMISSION: return "CLAIM_SUBMISSION";
        case TX_CLAIM_APPROVAL: return "CLAIM_APPROVAL";
        case TX_CLAIM_REJECTION: return "CLAIM_REJECTION";
        case TX_CLAIM_SETTLEMENT: return "CLAIM_SETTLEMENT";
        case TX_TOKEN_TRANSFER: return "TOKEN_TRANSFER";
        case TX_MINING_REWARD: return "MINING_REWARD";
        case TX_POOL_REWARD: return "POOL_REWARD";
        default: return "UNKNOWN";
    }
}

/**
 * verify_transaction_signature - Verify transaction signature
 */
int verify_transaction_signature(const transaction_t *tx)
{
    (void)tx;
    /* Stub - will be implemented with ECDSA */
    return 1;
}

/**
 * sign_transaction - Sign a transaction
 */
void sign_transaction(transaction_t *tx, const unsigned char *private_key)
{
    (void)tx;
    (void)private_key;
    /* Stub - will be implemented with ECDSA */
}

/**
 * find_transaction_in_chain - Find transaction in blockchain
 */
int find_transaction_in_chain(const char *tx_id)
{
    (void)tx_id;
    /* Stub - implement chain search */
    return 0;
}

/**
 * find_transaction_in_mempool - Find transaction in mempool
 */
int find_transaction_in_mempool(const char *tx_id)
{
    (void)tx_id;
    /* Stub - implement mempool search */
    return 0;
}
