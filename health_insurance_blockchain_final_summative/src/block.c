#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "block.h"
#include "cryptography.h"
#include "transaction.h"
#include "utils.h"

/**
 * create_block - Create a new block
 */
block_t *create_block(uint32_t block_id,
                      const unsigned char *prev_hash,
                      uint32_t difficulty,
                      const char *miner_id)
{
    block_t *block = calloc(1, sizeof(block_t));
    if (!block)
        return NULL;

    block->block_id = block_id;
    block->timestamp = time(NULL);
    block->difficulty = difficulty;

    if (prev_hash)
        memcpy(block->previous_hash, prev_hash, SHA256_DIGEST_LENGTH);

    if (miner_id)
        strncpy(block->miner_id, miner_id, sizeof(block->miner_id) - 1);

    block->transaction_count = 0;
    block->nonce = 0;

    memset(block->merkle_root, 0, SHA256_DIGEST_LENGTH);
    memset(block->hash, 0, SHA256_DIGEST_LENGTH);

    return block;
}

/**
 * free_block - Free block memory
 */
void free_block(block_t *block)
{
    if (!block)
        return;

    for (uint32_t i = 0; i < block->transaction_count; i++) {
        if (block->transactions[i])
            free(block->transactions[i]);
    }

    free(block);
}

/**
 * compute_block_hash - Hash full block
 */
void compute_block_hash(const block_t *block, unsigned char *hash)
{
    unsigned char buffer[2048];
    size_t offset = 0;

    if (!block || !hash)
        return;

    memcpy(buffer + offset, &block->block_id, sizeof(block->block_id));
    offset += sizeof(block->block_id);

    memcpy(buffer + offset, &block->timestamp, sizeof(block->timestamp));
    offset += sizeof(block->timestamp);

    memcpy(buffer + offset, &block->nonce, sizeof(block->nonce));
    offset += sizeof(block->nonce);

    memcpy(buffer + offset, block->previous_hash, SHA256_DIGEST_LENGTH);
    offset += SHA256_DIGEST_LENGTH;

    memcpy(buffer + offset, block->merkle_root, SHA256_DIGEST_LENGTH);
    offset += SHA256_DIGEST_LENGTH;

    sha256_hash(buffer, offset, hash);
}
