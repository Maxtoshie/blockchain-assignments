#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "mining.h"
#include "cryptography.h"
#include "blockchain.h"
#include "mempool.h"
#include "account.h"

/**
 * mine_solo - Solo mining implementation
 */
block_t *mine_solo(mempool_t *mempool, const char *miner_id)
{
    block_t *block;
    uint64_t nonce = 0;
    unsigned char hash[SHA256_DIGEST_LENGTH];
    unsigned char target[SHA256_DIGEST_LENGTH];
    chain_state_t *state;
    
    if (!mempool || !miner_id) {
        printf("Invalid parameters\n");
        return NULL;
    }
    
    state = get_chain_state();
    block = create_block(get_chain_length(), 
                         get_chain_tail() ? get_chain_tail()->hash : NULL,
                         state->difficulty, miner_id);
    if (!block) {
        printf("Failed to create block\n");
        return NULL;
    }
    
    /* Get transactions from mempool */
    mempool_entry_t *entries = get_top_transactions(mempool, 
        MAX_TRANSACTIONS_PER_BLOCK);
    
    int tx_count = 0;
    mempool_entry_t *current = entries;
    while (current && tx_count < MAX_TRANSACTIONS_PER_BLOCK) {
        if (current->status == STATUS_PENDING) {
            block->transactions[tx_count++] = current->tx;
        }
        current = current->next;
    }
    block->transaction_count = tx_count;
    
    /* Compute Merkle root */
    compute_merkle_root(block->transactions, tx_count, block->merkle_root);
    
    /* Compute target */
    compute_target(state->difficulty, target);
    
    printf("Mining block with difficulty %u...\n", state->difficulty);
    
    /* Proof of Work */
    while (1) {
        block->nonce = nonce;
        compute_block_hash(block, hash);
        
        if (hash_meets_target(hash, target)) {
            memcpy(block->hash, hash, SHA256_DIGEST_LENGTH);
            printf("Block found! Nonce: %lu\n", nonce);
            printf("Hash: ");
            print_hash(hash);
            printf("\n");
            break;
        }
        
        nonce++;
        if (nonce % 1000000 == 0) {
            printf("Hashed %lu nonces...\n", nonce);
        }
    }
    
    /* Add mining reward */
    transaction_t *reward = create_transaction("SYSTEM", miner_id,
        state->block_reward, TX_MINING_REWARD);
    if (reward) {
        block->transactions[tx_count] = reward;
        block->transaction_count++;
    }
    
    /* Add block to chain */
    if (add_block_to_chain(block) == 0) {
        /* Remove mined transactions from mempool */
        for (uint32_t i = 0; i < block->transaction_count; i++) {
            remove_from_mempool(mempool, block->transactions[i]->transaction_id);
        }
        printf("Block %u mined successfully\n", block->block_id);
        free_mempool_entries(entries);
        return block;
    }
    
    free_block(block);
    free_mempool_entries(entries);
    return NULL;
}

/**
 * mine_pool - Pool mining implementation
 */
block_t *mine_pool(mempool_t *mempool, const char *pool_address, 
                   char **miner_addresses, int num_miners)
{
    (void)miner_addresses;
    /* Simplified pool mining - just call solo mining with pool address */
    printf("Pool mining with %d miners\n", num_miners);
    return mine_solo(mempool, pool_address);
}

/**
 * retarget_difficulty - Automatic difficulty retargeting
 */
void retarget_difficulty(void)
{
    /* Stub - will be implemented fully */
    printf("Difficulty retargeting...\n");
}
