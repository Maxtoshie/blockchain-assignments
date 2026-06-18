#ifndef MINING_H
#define MINING_H

#include "blockchain.h"
#include "mempool.h"
#include <stdint.h>

/* =========================
 * Mining API
 * ========================= */
block_t *mine_solo(mempool_t *mempool, const char *miner_id);

block_t *mine_pool(mempool_t *mempool,
                   const char *pool_address,
                   char **miner_addresses,
                   int num_miners);

/* Difficulty adjustment */
void retarget_difficulty(void);

#endif /* MINING_H */
