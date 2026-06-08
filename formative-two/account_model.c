#include "blockchain.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Account *account_list = NULL;

/* Initialize account model */
void init_account_model(void) {
    account_list = NULL;
    Student *student = student_registry;
    
    while (student) {
        Account *new_account = malloc(sizeof(Account));
        if (!new_account) return;
        
        strcpy(new_account->student_id, student->id);
        new_account->balance = 0;
        new_account->nonce = 0;
        new_account->history = NULL;
        new_account->next = account_list;
        account_list = new_account;
        
        student = student->next;
    }
    printf("Account model initialized.\n");
}

/* Get account by student ID */
Account* get_account(const char *student_id) {
    Account *curr = account_list;
    while (curr) {
        if (strcmp(curr->student_id, student_id) == 0)
            return curr;
        curr = curr->next;
    }
    return NULL;
}

/* Add transaction to history */
void add_transaction_history(Account *account, const char *from, const char *to, 
                             int amount, int fee, int nonce) {
    TransactionLog *new_log = malloc(sizeof(TransactionLog));
    if (!new_log) return;
    
    strcpy(new_log->from_id, from);
    strcpy(new_log->to_id, to);
    new_log->amount = amount;
    new_log->fee = fee;
    new_log->nonce = nonce;
    new_log->timestamp = time(NULL);
    new_log->next = account->history;
    account->history = new_log;
}

/* Create transaction in account model */
int create_transaction_account(const char *from, const char *to, int amount) {
    if (amount <= 0) {
        printf("Invalid amount.\n");
        return 0;
    }
    
    Account *from_account = get_account(from);
    Account *to_account = get_account(to);
    
    if (!from_account || !to_account) {
        printf("Invalid account.\n");
        return 0;
    }
    
    int total_needed = amount + TRANSACTION_FEE;
    
    if (from_account->balance < total_needed) {
        printf("Insufficient balance! Need %d, have %d\n", 
               total_needed, from_account->balance);
        return 0;
    }
    
    /* Check nonce (simplified - expecting nonce = transaction count) */
    int expected_nonce = from_account->nonce;
    
    /* Perform transaction */
    from_account->balance -= total_needed;
    to_account->balance += amount;
    
    /* Add to history */
    add_transaction_history(from_account, from, to, amount, TRANSACTION_FEE, expected_nonce);
    add_transaction_history(to_account, from, to, amount, TRANSACTION_FEE, expected_nonce);
    
    from_account->nonce++;
    
    printf("✓ Transaction: %d tokens from %s to %s (fee: %d, nonce: %d)\n", 
           amount, from, to, TRANSACTION_FEE, expected_nonce);
    return 1;
}

/* Print account balance */
void print_account_balance(const char *student_id) {
    Account *acc = get_account(student_id);
    if (acc) {
        printf("%s balance: %d tokens (nonce: %d)\n", 
               student_id, acc->balance, acc->nonce);
    }
}

/* Print transaction history */
void print_transaction_history(const char *student_id) {
    Account *acc = get_account(student_id);
    if (!acc || !acc->history) {
        printf("No transaction history for %s\n", student_id);
        return;
    }
    
    printf("\n=== Transaction History for %s ===\n", student_id);
    TransactionLog *curr = acc->history;
    int count = 1;
    while (curr) {
        char *time_str = ctime(&curr->timestamp);
        /* Remove newline from ctime string */
        if (time_str) {
            int len = strlen(time_str);
            if (len > 0 && time_str[len - 1] == '\n') {
                time_str[len - 1] = '\0';
            }
        }
        printf("%d. %s -> %s: %d tokens (fee: %d, nonce: %d) %s\n",
               count++, curr->from_id, curr->to_id, curr->amount, 
               curr->fee, curr->nonce, time_str ? time_str : "");
        curr = curr->next;
    }
}
