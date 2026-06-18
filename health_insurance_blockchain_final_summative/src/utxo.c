#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "utxo.h"

#define MAX_UTXOS 10000

static utxo_t *utxo_set[MAX_UTXOS];
static int utxo_count = 0;

/**
 * create_utxo - Create a new UTXO
 */
utxo_t *create_utxo(const char *tx_id, uint32_t index, 
                    double amount, const char *owner)
{
    utxo_t *utxo;
    
    if (utxo_count >= MAX_UTXOS) {
        printf("UTXO set is full\n");
        return NULL;
    }
    
    utxo = (utxo_t *)calloc(1, sizeof(utxo_t));
    if (!utxo)
        return NULL;
    
    strncpy(utxo->tx_id, tx_id, sizeof(utxo->tx_id) - 1);
    utxo->output_index = index;
    utxo->amount = amount;
    strncpy(utxo->owner, owner, MAX_ADDRESS_LENGTH - 1);
    utxo->spent = false;
    
    utxo_set[utxo_count++] = utxo;
    return utxo;
}

/**
 * free_utxo - Free a UTXO
 */
void free_utxo(utxo_t *utxo)
{
    if (utxo)
        free(utxo);
}

/**
 * add_utxo - Add a UTXO to the set
 */
int add_utxo(utxo_t *utxo)
{
    if (!utxo)
        return -1;
    
    if (utxo_count >= MAX_UTXOS)
        return -1;
    
    utxo_set[utxo_count++] = utxo;
    return 0;
}

/**
 * spend_utxo - Mark a UTXO as spent
 */
int spend_utxo(const char *tx_id, uint32_t index)
{
    utxo_t *utxo = find_utxo(tx_id, index);
    if (!utxo)
        return -1;
    
    if (utxo->spent) {
        printf("UTXO already spent\n");
        return -1;
    }
    
    utxo->spent = true;
    return 0;
}

/**
 * find_utxo - Find a UTXO by transaction ID and index
 */
utxo_t *find_utxo(const char *tx_id, uint32_t index)
{
    for (int i = 0; i < utxo_count; i++) {
        if (strcmp(utxo_set[i]->tx_id, tx_id) == 0 && 
            utxo_set[i]->output_index == index && !utxo_set[i]->spent) {
            return utxo_set[i];
        }
    }
    return NULL;
}

/**
 * get_utxo_balance - Get total UTXO balance for an address
 */
double get_utxo_balance(const char *address)
{
    double balance = 0.0;
    
    for (int i = 0; i < utxo_count; i++) {
        if (strcmp(utxo_set[i]->owner, address) == 0 && !utxo_set[i]->spent) {
            balance += utxo_set[i]->amount;
        }
    }
    
    return balance;
}

/**
 * display_utxo_set - Display all UTXOs
 */
void display_utxo_set(void)
{
    printf("\n=== UTXO SET ===\n");
    printf("Total UTXOs: %d\n", utxo_count);
    
    for (int i = 0; i < utxo_count; i++) {
        printf("%d. TX: %s, Index: %u, Amount: %.2f, Owner: %s, Spent: %s\n",
               i + 1, utxo_set[i]->tx_id, utxo_set[i]->output_index,
               utxo_set[i]->amount, utxo_set[i]->owner,
               utxo_set[i]->spent ? "YES" : "NO");
    }
}
