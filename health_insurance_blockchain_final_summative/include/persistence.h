#ifndef PERSISTENCE_H
#define PERSISTENCE_H

#include "blockchain.h"
#include "mempool.h"

int save_blockchain(void);
int load_blockchain(void);
int save_blockchain_to_file(const char *filename);
int load_blockchain_from_file(const char *filename);
int save_chain_state(chain_state_t *state);
int load_chain_state(void);

#endif /* PERSISTENCE_H */
