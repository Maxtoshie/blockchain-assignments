#include "blockchain.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

UTXO *utxo_list = NULL;

/* Initialize UTXO model */
void init_utxo_model(void) {
    utxo_list = NULL;
    printf("UTXO model initialized.\n");
}

/* Add UTXO to list */
void add_utxo(const char *tx_id, const char *owner, int amount) {
    UTXO *new_utxo = malloc(sizeof(UTXO));
    if (!new_utxo) return;
    
    strcpy(new_utxo->tx_id, tx_id);
    strcpy(new_utxo->owner_id, owner);
    new_utxo->amount = amount;
    new_utxo->is_spent = 0;
    new_utxo->next = utxo_list;
    utxo_list = new_utxo;
}

/* Get balance from UTXOs */
int get_balance_utxo(const char *student_id) {
    int balance = 0;
    UTXO *curr = utxo_list;
    while (curr) {
        if (strcmp(curr->owner_id, student_id) == 0 && !curr->is_spent)
            balance += curr->amount;
        curr = curr->next;
    }
    return balance;
}

/* Create transaction in UTXO model */
int create_transaction_utxo(const char *from, const char *to, int amount) {
    if (amount <= 0) {
        printf("Invalid amount.\n");
        return 0;
    }
    
    /* Calculate available balance */
    int available = get_balance_utxo(from);
    int total_needed = amount + TRANSACTION_FEE;
    
    if (available < total_needed) {
        printf("Insufficient balance! Need %d, have %d\n", total_needed, available);
        return 0;
    }
    
    /* Collect UTXOs for spending */
    UTXO *curr = utxo_list;
    int collected = 0;
    char tx_id[65];
    char tx_input[256];
    
    sprintf(tx_input, "%s%s%d%ld", from, to, amount, time(NULL));
    init_sha256(tx_id, tx_input);
    
    /* Mark UTXOs as spent and create new ones */
    while (curr && collected < total_needed) {
        if (strcmp(curr->owner_id, from) == 0 && !curr->is_spent) {
            collected += curr->amount;
            curr->is_spent = 1;
            
            /* Create output UTXO for recipient */
            add_utxo(tx_id, to, amount);
            
            /* Create change UTXO if any */
            int change = collected - total_needed;
            if (change > 0) {
                add_utxo(tx_id, from, change);
            }
        }
        curr = curr->next;
    }
    
    printf("✓ Transaction: %d tokens from %s to %s (fee: %d)\n", 
           amount, from, to, TRANSACTION_FEE);
    return 1;
}

/* Print UTXO set */
void print_utxo_set(void) {
    printf("\n=== UTXO SET ===\n");
    UTXO *curr = utxo_list;
    int count = 1;
    while (curr) {
        if (!curr->is_spent) {
            printf("%d. TX: %s... | Owner: %s | Amount: %d\n",
                   count++, curr->tx_id, curr->owner_id, curr->amount);
        }
        curr = curr->next;
    }
    
    /* Print balances */
    printf("\n--- Balances ---\n");
    Student *student = student_registry;
    while (student) {
        printf("%s (%s): %d tokens\n", student->name, student->id, 
               get_balance_utxo(student->id));
        student = student->next;
    }
}
