#ifndef BLOCK_H
#define BLOCK_H

#include <stdint.h>
#include <time.h>
#include "transaction.h"

#define SHA256_DIGEST_LENGTH 32
#define MAX_TRANSACTIONS_PER_BLOCK 100

/**
 * struct block - Represents a block in the blockchain
 */
typedef struct block {
    uint32_t block_id;
    time_t timestamp;
    uint32_t transaction_count;
    unsigned char previous_hash[SHA256_DIGEST_LENGTH];
    unsigned char merkle_root[SHA256_DIGEST_LENGTH];
    unsigned char hash[SHA256_DIGEST_LENGTH];
    uint64_t nonce;
    char miner_id[64];
    uint32_t difficulty;
    transaction_t *transactions[MAX_TRANSACTIONS_PER_BLOCK];
    struct block *next;
} block_t;

/* Function prototypes */
block_t *create_block(uint32_t block_id, const unsigned char *prev_hash,
                      uint32_t difficulty, const char *miner_id);

void free_block(block_t *block);

/* FIXED: return type must match implementation */
void compute_block_hash(const block_t *block, unsigned char *hash);

int verify_block_hash(const block_t *block);

/* NOTE: should ONLY exist in cryptography.c but kept if used */
void print_hash(const unsigned char *hash);

#endif /* BLOCK_H */
