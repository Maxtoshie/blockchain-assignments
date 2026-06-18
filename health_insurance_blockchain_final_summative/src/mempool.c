#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mempool.h"

static mempool_t *global_mempool = NULL;

/**
 * create_mempool - Create a new mempool
 */
mempool_t *create_mempool(void)
{
    mempool_t *mempool;
    
    mempool = (mempool_t *)calloc(1, sizeof(mempool_t));
    if (!mempool)
        return NULL;
    
    mempool->head = NULL;
    mempool->size = 0;
    mempool->max_size = MAX_MEMPOOL_SIZE;
    
    return mempool;
}

/**
 * free_mempool - Free a mempool
 */
void free_mempool(mempool_t *mempool)
{
    mempool_entry_t *current, *next;
    
    if (!mempool)
        return;
    
    current = mempool->head;
    while (current) {
        next = current->next;
        if (current->tx)
            free_transaction(current->tx);
        free(current);
        current = next;
    }
    
    free(mempool);
}

/**
 * get_global_mempool - Get the global mempool
 */
mempool_t *get_global_mempool(void)
{
    return global_mempool;
}

/**
 * initialize_mempool - Initialize the global mempool
 */
void initialize_mempool(void)
{
    if (!global_mempool)
        global_mempool = create_mempool();
}

/**
 * add_to_mempool - Add a transaction to the mempool
 */
int add_to_mempool(mempool_t *mempool, transaction_t *tx, double fee)
{
    return add_to_mempool_with_status(mempool, tx, fee, STATUS_PENDING);
}

/**
 * add_to_mempool_with_status - Add transaction with specific status
 */
int add_to_mempool_with_status(mempool_t *mempool, transaction_t *tx, 
                                double fee, mempool_status_t status)
{
    mempool_entry_t *entry;
    
    if (!mempool || !tx)
        return -1;
    
    if (mempool->size >= mempool->max_size) {
        printf("Mempool is full\n");
        return -1;
    }
    
    entry = (mempool_entry_t *)calloc(1, sizeof(mempool_entry_t));
    if (!entry)
        return -1;
    
    entry->tx = tx;
    entry->fee = fee;
    entry->status = status;
    entry->next = mempool->head;
    mempool->head = entry;
    mempool->size++;
    
    /* Sort mempool by fee */
    sort_mempool(mempool);
    
    return 0;
}

/**
 * remove_from_mempool - Remove a transaction from the mempool
 */
int remove_from_mempool(mempool_t *mempool, const char *tx_id)
{
    mempool_entry_t *current, *prev = NULL;
    
    if (!mempool || !tx_id)
        return -1;
    
    current = mempool->head;
    while (current) {
        if (strcmp(current->tx->transaction_id, tx_id) == 0) {
            if (prev)
                prev->next = current->next;
            else
                mempool->head = current->next;
            
            free_transaction(current->tx);
            free(current);
            mempool->size--;
            return 0;
        }
        prev = current;
        current = current->next;
    }
    
    return -1;
}

/**
 * sort_mempool - Sort mempool by fee (descending) and timestamp (ascending)
 */
void sort_mempool(mempool_t *mempool)
{
    mempool_entry_t *sorted = NULL;
    mempool_entry_t *current = mempool->head;
    mempool_entry_t *next;
    
    if (!mempool || !mempool->head)
        return;
    
    while (current) {
        next = current->next;
        
        /* Insert into sorted list */
        if (!sorted || current->fee > sorted->fee ||
            (current->fee == sorted->fee && current->tx->timestamp < sorted->tx->timestamp)) {
            current->next = sorted;
            sorted = current;
        } else {
            mempool_entry_t *temp = sorted;
            while (temp->next && (temp->next->fee > current->fee ||
                   (temp->next->fee == current->fee && 
                    temp->next->tx->timestamp < current->tx->timestamp))) {
                temp = temp->next;
            }
            current->next = temp->next;
            temp->next = current;
        }
        
        current = next;
    }
    
    mempool->head = sorted;
}

/**
 * get_top_transactions - Get top N transactions from mempool
 */
mempool_entry_t *get_top_transactions(mempool_t *mempool, int count)
{
    mempool_entry_t *result = NULL;
    mempool_entry_t *current = mempool->head;
    mempool_entry_t *new_entry;
    int i = 0;
    
    if (!mempool || !mempool->head)
        return NULL;
    
    sort_mempool(mempool);
    
    while (current && i < count) {
        if (current->status == STATUS_PENDING) {
            new_entry = (mempool_entry_t *)malloc(sizeof(mempool_entry_t));
            if (!new_entry)
                break;
            
            new_entry->tx = current->tx;
            new_entry->fee = current->fee;
            new_entry->status = current->status;
            new_entry->next = result;
            result = new_entry;
            i++;
        }
        current = current->next;
    }
    
    return result;
}

/**
 * display_mempool - Display mempool contents
 */
void display_mempool(const mempool_t *mempool)
{
    mempool_entry_t *current;
    int count = 0;
    
    if (!mempool) {
        printf("Mempool not initialized\n");
        return;
    }
    
    printf("\n=== MEMPOOL ===\n");
    printf("Total transactions: %d\n", mempool->size);
    printf("Max size: %d\n", mempool->max_size);
    printf("\n");
    
    if (!mempool->head) {
        printf("Mempool is empty\n");
        return;
    }
    
    sort_mempool((mempool_t *)mempool);
    current = mempool->head;
    
    while (current) {
        printf("%d. TX ID: %s\n", ++count, current->tx->transaction_id);
        printf("   From: %s\n", current->tx->sender_address);
        printf("   To: %s\n", current->tx->receiver_address);
        printf("   Amount: %.2f AHT\n", current->tx->amount);
        printf("   Type: %s\n", transaction_type_to_string(current->tx->type));
        printf("   Fee: %.6f\n", current->fee);
        printf("   Status: %s\n", 
               current->status == STATUS_PENDING ? "PENDING" :
               current->status == STATUS_CONFIRMED ? "CONFIRMED" : "SUSPICIOUS");
        printf("   Timestamp: %s", ctime(&current->tx->timestamp));
        printf("----------------------------------------\n");
        current = current->next;
    }
}

/**
 * free_mempool_entries - Free mempool entries list
 */
void free_mempool_entries(mempool_entry_t *entries)
{
    mempool_entry_t *current = entries;
    mempool_entry_t *next;
    
    while (current) {
        next = current->next;
        free(current);
        current = next;
    }
}
