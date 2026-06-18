#ifndef UTXO_H
#define UTXO_H

#include <stdint.h>
#include <stdbool.h>
#include "transaction.h"

/**
 * struct utxo - Unspent Transaction Output
 */
typedef struct utxo {
    char tx_id[64];
    uint32_t output_index;
    double amount;
    char owner[MAX_ADDRESS_LENGTH];
    bool spent;
} utxo_t;

/* Function prototypes */
utxo_t *create_utxo(const char *tx_id, uint32_t index, 
                    double amount, const char *owner);
void free_utxo(utxo_t *utxo);
int add_utxo(utxo_t *utxo);
int spend_utxo(const char *tx_id, uint32_t index);
utxo_t *find_utxo(const char *tx_id, uint32_t index);
double get_utxo_balance(const char *address);
void display_utxo_set(void);

#endif /* UTXO_H */
