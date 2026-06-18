#ifndef BLOCKCHAIN_H
#define BLOCKCHAIN_H

#include <stdint.h>
#include <time.h>
#include "block.h"
#include "transaction.h"

/**
 * struct chain_state - Global blockchain state
 */
typedef struct chain_state {
    uint32_t difficulty;
    double block_reward;
    uint32_t last_retarget_block;
    uint32_t total_blocks;
    uint32_t genesis_block_id;
} chain_state_t;

/* Function prototypes */
void init_chain_state(void);
void set_chain_state(chain_state_t *state);
chain_state_t *get_chain_state(void);
block_t *create_genesis_block(void);
int add_block_to_chain(block_t *block);
int validate_block(const block_t *block);
int verify_blockchain(void);
block_t *get_block_by_id(uint32_t id);
block_t *get_chain_head(void);
block_t *get_chain_tail(void);
uint32_t get_chain_length(void);
void display_blockchain(void);
void display_difficulty_status(void);
int initialize_blockchain(void);

#endif /* BLOCKCHAIN_H */
