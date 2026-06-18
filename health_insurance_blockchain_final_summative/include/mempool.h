#ifndef MEMPOOL_H
#define MEMPOOL_H

#include "transaction.h"
#include <stdbool.h>

#define MAX_MEMPOOL_SIZE 1000

/**
 * enum mempool_status - Status of a transaction in the mempool
 */
typedef enum {
    STATUS_PENDING,
    STATUS_CONFIRMED,
    STATUS_SUSPICIOUS
} mempool_status_t;

/**
 * struct mempool_entry - Entry in the mempool
 */
typedef struct mempool_entry {
    transaction_t *tx;
    double fee;
    mempool_status_t status;
    struct mempool_entry *next;
} mempool_entry_t;

/**
 * struct mempool - Mempool structure
 */
typedef struct mempool {
    mempool_entry_t *head;
    int size;
    int max_size;
} mempool_t;

/* Function prototypes */
mempool_t *create_mempool(void);
void free_mempool(mempool_t *mempool);
mempool_t *get_global_mempool(void);
void initialize_mempool(void);
int add_to_mempool(mempool_t *mempool, transaction_t *tx, double fee);
int add_to_mempool_with_status(mempool_t *mempool, transaction_t *tx, 
                                double fee, mempool_status_t status);
int remove_from_mempool(mempool_t *mempool, const char *tx_id);
mempool_entry_t *get_top_transactions(mempool_t *mempool, int count);
void sort_mempool(mempool_t *mempool);
void display_mempool(const mempool_t *mempool);
void free_mempool_entries(mempool_entry_t *entries);

#endif /* MEMPOOL_H */
