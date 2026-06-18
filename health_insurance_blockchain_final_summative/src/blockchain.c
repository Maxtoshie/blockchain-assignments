#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "blockchain.h"
#include "cryptography.h"
#include "mempool.h"
#include "persistence.h"
#include "account.h"
#include "mining.h"

/* =========================
 * GLOBAL CHAIN STATE
 * ========================= */
static block_t *chain_head = NULL;
static block_t *chain_tail = NULL;
static uint32_t chain_length = 0;
static chain_state_t chain_state;

/* =========================
 * CHAIN STATE INIT
 * ========================= */
void init_chain_state(void)
{
    memset(&chain_state, 0, sizeof(chain_state_t));
    chain_state.difficulty = 2;
    chain_state.block_reward = 10.0;
    chain_state.last_retarget_block = 0;
    chain_state.total_blocks = 0;
    chain_state.genesis_block_id = 0;
}

void set_chain_state(chain_state_t *state)
{
    if (state)
        chain_state = *state;
}

chain_state_t *get_chain_state(void)
{
    return &chain_state;
}

/* =========================
 * CHAIN ACCESSORS
 * ========================= */
block_t *get_chain_head(void) { return chain_head; }
block_t *get_chain_tail(void) { return chain_tail; }
uint32_t get_chain_length(void) { return chain_length; }

block_t *get_block_by_id(uint32_t id)
{
    block_t *current = chain_head;

    while (current) {
        if (current->block_id == id)
            return current;
        current = current->next;
    }

    return NULL;
}

/* =========================
 * GENESIS BLOCK
 * ========================= */
block_t *create_genesis_block(void)
{
    unsigned char zero_hash[SHA256_DIGEST_LENGTH] = {0};

    block_t *genesis = create_block(
        0,
        zero_hash,
        chain_state.difficulty,
        "GENESIS"
    );

    if (!genesis)
        return NULL;

    transaction_t *initial_tx = create_transaction(
        "GENESIS",
        "INSURANCE_POOL",
        1000000.0,
        TX_TOKEN_TRANSFER
    );

    if (initial_tx) {
        genesis->transactions[0] = initial_tx;
        genesis->transaction_count = 1;
    }

    genesis->difficulty = 1;

    compute_merkle_root(genesis->transactions,
                        genesis->transaction_count,
                        genesis->merkle_root);

    compute_block_hash(genesis, genesis->hash);

    chain_head = genesis;
    chain_tail = genesis;
    chain_length = 1;
    chain_state.total_blocks = 1;

    return genesis;
}

/* =========================
 * ADD BLOCK
 * ========================= */
int add_block_to_chain(block_t *block)
{
    if (!block || validate_block(block) != 0)
        return -1;

    block->block_id = chain_length;

    if (chain_tail)
        memcpy(block->previous_hash, chain_tail->hash, SHA256_DIGEST_LENGTH);

    if (chain_tail)
        chain_tail->next = block;

    chain_tail = block;
    chain_length++;
    chain_state.total_blocks++;

    if (chain_length % 10 == 0)
        retarget_difficulty();

    save_chain_state(&chain_state);
    save_blockchain();

    return 0;
}

/* =========================
 * BLOCK VALIDATION (FIXED PoW)
 * ========================= */
int validate_block(const block_t *block)
{
    unsigned char computed_hash[SHA256_DIGEST_LENGTH];
    unsigned char computed_merkle[SHA256_DIGEST_LENGTH];
    unsigned char target[SHA256_DIGEST_LENGTH];

    if (!block)
        return -1;

    /* Recompute block hash */
    compute_block_hash(block, computed_hash);

    if (memcmp(computed_hash, block->hash, SHA256_DIGEST_LENGTH) != 0)
        return -1;

    /* =========================
     * FIXED PROOF OF WORK
     * ========================= */
    compute_target(block->difficulty, target);

    if (!verify_pow(computed_hash, target))
        return -1;

    /* Verify merkle root */
    compute_merkle_root(block->transactions,
                         block->transaction_count,
                         computed_merkle);

    if (memcmp(computed_merkle, block->merkle_root, SHA256_DIGEST_LENGTH) != 0)
        return -1;

    /* Verify chain linkage */
    if (block->block_id > 0) {
        block_t *prev = get_block_by_id(block->block_id - 1);

        if (!prev)
            return -1;

        if (memcmp(block->previous_hash,
                   prev->hash,
                   SHA256_DIGEST_LENGTH) != 0)
            return -1;
    }

    /* Verify transactions */
    for (uint32_t i = 0; i < block->transaction_count; i++) {
        if (!verify_transaction_signature(block->transactions[i]))
            return -1;
    }

    return 0;
}

/* =========================
 * BLOCKCHAIN VALIDATION
 * ========================= */
int verify_blockchain(void)
{
    block_t *current = chain_head;
    uint32_t count = 0;

    if (!current) {
        printf("Blockchain is empty\n");
        return -1;
    }

    while (current) {
        if (validate_block(current) != 0) {
            printf("Block %u failed validation\n", count);
            return -1;
        }

        current = current->next;
        count++;
    }

    printf("Blockchain verified: %u blocks OK\n", count);
    return 0;
}

/* =========================
 * DISPLAY CHAIN
 * ========================= */
void display_blockchain(void)
{
    block_t *current = chain_head;
    int count = 0;

    printf("\n=== BLOCKCHAIN ===\n");
    printf("Total blocks: %u\n", chain_length);
    printf("Difficulty: %u\n", chain_state.difficulty);
    printf("Reward: %.2f\n\n", chain_state.block_reward);

    while (current) {
        printf("Block #%u\n", current->block_id);
        printf("  Time: %s", ctime(&current->timestamp));
        printf("  TXs: %u\n", current->transaction_count);
        printf("  Nonce: %lu\n", (unsigned long)current->nonce);
        printf("  Miner: %s\n", current->miner_id);
        printf("  Hash: ");
        print_hash(current->hash);
        printf("\n\n");

        current = current->next;
        count++;

        if (count >= 10) {
            printf("... %u more blocks\n", chain_length - count);
            break;
        }
    }
}

/* =========================
 * DIFFICULTY STATUS
 * ========================= */
void display_difficulty_status(void)
{
    printf("\n=== DIFFICULTY ===\n");
    printf("Difficulty: %u\n", chain_state.difficulty);
    printf("Last retarget: %u\n", chain_state.last_retarget_block);
    printf("Total blocks: %u\n", chain_state.total_blocks);
}

/* =========================
 * INIT
 * ========================= */
int initialize_blockchain(void)
{
    if (!chain_head)
        init_chain_state();

    return 0;
}
